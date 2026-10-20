
#include "html.h"
#include "lexer.h"

#include <stdio.h>
#include <string.h>

static void write_escaped(FILE *dest, const char *text)
{
    while (*text != '\0')
    {
        write_special_char(dest, (unsigned char)*text);
        text++;
    }
}


static void write_preprocessor_token(FILE *dest, const char *text)
{
    int i = 0;
    int start;

    /* Color the preprocessor directive purple */
    while (text[i] != '\0' &&
           text[i] != ' ' &&
           text[i] != '\t' &&
           text[i] != '\n')
    {
        i++;
    }

    fprintf(dest, "<span style=\"color:#C586C0;\">");
    for (start = 0; start < i; start++)
    {
        write_special_char(dest, (unsigned char)text[start]);
    }
    fprintf(dest, "</span>");

    /* Preserve whitespace after the directive */
    while (text[i] == ' ' || text[i] == '\t')
    {
        write_special_char(dest, (unsigned char)text[i]);
        i++;
    }

    /* Color header filenames blue */
    if (strcmp(text, "#include") == 0 ||
        strncmp(text, "#include ", 9) == 0 ||
        strncmp(text, "#include\t", 9) == 0)
    {
        if (text[i] == '<' || text[i] == '"')
        {
            char closing = (text[i] == '<') ? '>' : '"';

            fprintf(dest, "<span style=\"color:#569CD6;\">");

            while (text[i] != '\0')
            {
                char ch = text[i++];
                write_special_char(dest, (unsigned char)ch);

                if (ch == closing)
                {
                    break;
                }
            }

            fprintf(dest, "</span>");
        }
    }

    /* Write anything remaining exactly once */
    write_escaped(dest, text + i);
}


static void write_token(FILE *dest, const Token *token)
{
    const char *color = NULL;

    switch (token->type)
    {
        case TOKEN_KEYWORD:
            color = "#569CD6";
            break;

        case TOKEN_NUMBER:
            color = "#B5CEA8";
            break;

        case TOKEN_STRING:
        case TOKEN_CHARACTER:
            color = "#CE9178";
            break;

        case TOKEN_COMMENT:
            color = "#6A9955";
            break;

        case TOKEN_IDENTIFIER:
            if (strcmp(token->value, "main") == 0 ||
                strcmp(token->value, "printf") == 0)
            {
                color = "#DCDCAA";
            }
            break;

        default:
            break;
    }

    if (token->type == TOKEN_WHITESPACE)
    {
        write_escaped(dest, token->value);
    }
    else if (token->type == TOKEN_PREPROCESSOR)
    {
        write_preprocessor_token(dest, token->value);
    }
    else if (color != NULL)
    {
        fprintf(dest, "<span style=\"color:%s;\">", color);
        write_escaped(dest, token->value);
        fprintf(dest, "</span>");
    }
    else
    {
        write_escaped(dest, token->value);
    }
}

int main(int argc, char *argv[])
{
    FILE *src;
    FILE *dest;
    Token token;

    if (argc != 2)
    {
        printf("Usage: %s <source.c>\n", argv[0]);
        return 1;
    }

    src = fopen(argv[1], "r");

    if (src == NULL)
    {
        perror("Error opening source file");
        return 1;
    }

    dest = fopen("output.html", "w");

    if (dest == NULL)
    {
        perror("Error creating HTML file");
        fclose(src);
        return 1;
    }

    fprintf(dest,
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<meta charset=\"UTF-8\">\n"
            "<title>Source Code</title>\n"
            "<style>\n"
            "body { background:#1e1e1e; color:#d4d4d4; }\n"
            "pre { font-size:16px; padding:20px; "
            "white-space:pre-wrap; }\n"
            "</style>\n"
            "</head>\n"
            "<body>\n<pre>");

    while (get_next_token(src, &token))
    {
        write_token(dest, &token);
    }

    fprintf(dest,
            "\n</pre>\n"
            "</body>\n"
            "</html>\n");

    fclose(src);
    fclose(dest);

    printf("HTML file generated successfully: output.html\n");

    return 0;
}
