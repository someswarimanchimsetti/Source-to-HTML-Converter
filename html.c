#include "html.h"
#include <string.h>
#include <ctype.h>

char macros[100][100];
int macro_count = 0;
void write_special_char(FILE *dest, int ch);


int is_keyword(char *word)
{
    static const char *keywords[] = {
        "auto", "break", "case", "char", "const", "continue",
        "default", "do", "double", "else", "enum", "extern",
        "float", "for", "goto", "if", "inline", "int", "long",
        "register", "restrict", "return", "short", "signed",
        "sizeof", "static", "struct", "switch", "typedef",
        "union", "unsigned", "void", "volatile", "while",
        "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex",
        "_Generic", "_Imaginary", "_Noreturn", "_Static_assert",
        "_Thread_local"
    };

    size_t count = sizeof(keywords) / sizeof(keywords[0]);

    for (size_t i = 0; i < count; i++)
    {
        if (strcmp(word, keywords[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}

int is_macro(char *word)
{
    int i;

    for (i = 0; i < macro_count; i++)
    {
        if (strcmp(word, macros[i]) == 0)
            return 1;
    }

    return 0;
}


void write_word(FILE *dest, char *word)
{
    if (is_keyword(word) || is_macro(word))
    {
        fprintf(dest,
                "<span style=\"color:#569CD6;\">%s</span>",
                word);
    }
    else if (strcmp(word, "main") == 0 ||
             strcmp(word, "printf") == 0)
    {
        fprintf(dest,
                "<span style=\"color:#DCDCAA;\">%s</span>",
                word);
    }
    else
    {
        fprintf(dest, "%s", word);
    }
}


void write_number(FILE *dest, char *number)
{
    fprintf(dest,
            "<span style=\"color:#B5CEA8;\">%s</span>",
            number);
}

void write_character(FILE *src, FILE *dest)
{
    int ch;
    int escape = 0;

    fprintf(dest, "<span style=\"color:#CE9178;\">'");

    while ((ch = fgetc(src)) != EOF)
    {
        if (ch == '\'' && !escape)
        {
            fprintf(dest, "'</span>");
            return;
        }

        write_special_char(dest, ch);

        if (escape)
            escape = 0;
        else if (ch == '\\')
            escape = 1;
    }

    fprintf(dest, "</span>");
}

void write_special_char(FILE *dest, int ch)
{
    if (ch == '<')
        fprintf(dest, "&lt;");
    else if (ch == '>')
        fprintf(dest, "&gt;");
    else if (ch == '&')
        fprintf(dest, "&amp;");
    else
        fputc(ch, dest);
}

void write_preprocessor(FILE *src, FILE *dest)
{
    int ch;
    char directive[100];
    int i = 0;

    fprintf(dest, "<span style=\"color:#C586C0;\">#");

    while ((ch = fgetc(src)) != EOF &&
           (isalpha((unsigned char)ch) || ch == '_'))
    {
        if (i < 99)
            directive[i++] = ch;
    }

    directive[i] = '\0';
fprintf(dest, "%s</span>", directive);

if (strcmp(directive, "define") == 0)
{
    int ch;

    while ((ch = fgetc(src)) != EOF &&
           isspace((unsigned char)ch) && ch != '\n')
    {
        fputc(ch, dest);
    }

    if (ch != EOF && (isalpha((unsigned char)ch) || ch == '_'))
    {
        char macro[100];
        int j = 0;

        do
        {
            if (j < 99)
                macro[j++] = ch;

            ch = fgetc(src);
        }
        while (ch != EOF &&
               (isalnum((unsigned char)ch) || ch == '_'));

        macro[j] = '\0';

        if (macro_count < 100 && !is_macro(macro))
{
    strcpy(macros[macro_count], macro);
    macro_count++;
}

        fprintf(dest,
                "<span style=\"color:#569CD6;\">%s</span>",
                macro);

        if (ch != EOF)
            ungetc(ch, src);
    }
}

    while (ch != EOF && ch != '\n')
    {
        if (ch == '<')
        {
            fprintf(dest,
                    "<span style=\"color:#569CD6;\">&lt;");

            while ((ch = fgetc(src)) != EOF && ch != '>')
                write_special_char(dest, ch);

            if (ch == '>')
                fprintf(dest, "&gt;</span>");
        }
        else if (ch == '"')
        {
            fprintf(dest,
                    "<span style=\"color:#CE9178;\">&quot;");

            while ((ch = fgetc(src)) != EOF)
            {
                write_special_char(dest, ch);

                if (ch == '"')
                    break;
            }

            fprintf(dest, "</span>");
        }
        else if (isalnum((unsigned char)ch) || ch == '_')
        {
            char word[100];
            int j = 0;

            do
            {
                if (j < 99)
                    word[j++] = ch;

                ch = fgetc(src);
            } while (ch != EOF &&
                     (isalnum((unsigned char)ch) || ch == '_'));

            word[j] = '\0';

            if (isdigit((unsigned char)word[0]))
                write_number(dest, word);
            else
                write_word(dest, word);

            continue;
        }
        else
        {
            write_special_char(dest, ch);
        }

        ch = fgetc(src);
    }

    if (ch == '\n')
        fputc('\n', dest);
}

