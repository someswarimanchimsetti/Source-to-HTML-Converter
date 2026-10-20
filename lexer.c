
#include "lexer.h"

#include <ctype.h>
#include <string.h>

/* Check whether a word is a C keyword */
static int is_keyword(const char *word)
{
    static const char *keywords[] =
    {
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

/* Check the first character of an identifier */
static int is_identifier_start(int ch)
{
    return isalpha((unsigned char)ch) || ch == '_';
}

/* Check the remaining characters of an identifier */
static int is_identifier_part(int ch)
{
    return isalnum((unsigned char)ch) || ch == '_';
}

/* Add a character without overflowing the token buffer */
static void add_char(Token *token, int ch)
{
    size_t len = strlen(token->value);

    if (len < TOKEN_SIZE - 1)
    {
        token->value[len] = (char)ch;
        token->value[len + 1] = '\0';
    }
}

/* Read a keyword or identifier */
static int read_identifier(FILE *src, Token *token, int first)
{
    int ch;

    add_char(token, first);

    while ((ch = fgetc(src)) != EOF)
    {
        if (!is_identifier_part(ch))
        {
            ungetc(ch, src);
            break;
        }

        add_char(token, ch);
    }

    token->type = is_keyword(token->value)
                  ? TOKEN_KEYWORD
                  : TOKEN_IDENTIFIER;

    return 1;
}

/* Read an integer or floating-point number */
static int read_number(FILE *src, Token *token, int first)
{
    int ch;
    int previous = first;

    add_char(token, first);

    while ((ch = fgetc(src)) != EOF)
    {
        if (isalnum((unsigned char)ch) ||
            ch == '.' || ch == '_')
        {
            add_char(token, ch);
            previous = ch;
        }
        else if ((ch == '+' || ch == '-') &&
                 (previous == 'e' || previous == 'E' ||
                  previous == 'p' || previous == 'P'))
        {
            add_char(token, ch);
            previous = ch;
        }
        else
        {
            ungetc(ch, src);
            break;
        }
    }

    token->type = TOKEN_NUMBER;
    return 1;
}

/* Read a string literal or character literal */
static int read_quoted(FILE *src, Token *token, int quote)
{
    int ch;

    add_char(token, quote);

    while ((ch = fgetc(src)) != EOF)
    {
        add_char(token, ch);

        if (ch == '\\')
        {
            ch = fgetc(src);

            if (ch == EOF)
            {
                break;
            }

            add_char(token, ch);
        }
        else if (ch == quote)
        {
            break;
        }
    }

    token->type = (quote == '"')
                  ? TOKEN_STRING
                  : TOKEN_CHARACTER;

    return 1;
}

/* Read line comments, block comments, or the division operator */
static int read_slash_token(FILE *src, Token *token)
{
    int ch = fgetc(src);

    add_char(token, '/');

    if (ch == '/')
    {
        add_char(token, ch);

        while ((ch = fgetc(src)) != EOF && ch != '\n')
        {
            add_char(token, ch);
        }

        if (ch == '\n')
        {
            ungetc(ch, src);
        }

        token->type = TOKEN_COMMENT;
    }
    else if (ch == '*')
    {
        int previous = 0;

        add_char(token, ch);

        while ((ch = fgetc(src)) != EOF)
        {
            add_char(token, ch);

            if (previous == '*' && ch == '/')
            {
                break;
            }

            previous = ch;
        }

        token->type = TOKEN_COMMENT;
    }
    else
    {
        if (ch != EOF)
        {
            ungetc(ch, src);
        }

        token->type = TOKEN_OPERATOR;
    }

    return 1;
}

/* Read common C operators */
static int read_operator(FILE *src, Token *token, int first)
{
    int ch;
    int next;

    add_char(token, first);

    ch = fgetc(src);

    if (ch == EOF)
    {
        token->type = TOKEN_OPERATOR;
        return 1;
    }

    if ((first == '+' && (ch == '+' || ch == '=')) ||
        (first == '-' && (ch == '-' || ch == '=' || ch == '>')) ||
        (first == '*' && ch == '=') ||
        (first == '%' && ch == '=') ||
        (first == '=' && ch == '=') ||
        (first == '!' && ch == '=') ||
        (first == '<' && (ch == '=' || ch == '<')) ||
        (first == '>' && (ch == '=' || ch == '>')) ||
        (first == '&' && (ch == '&' || ch == '=')) ||
        (first == '|' && (ch == '|' || ch == '=')) ||
        (first == '^' && ch == '='))
    {
        add_char(token, ch);

        if ((first == '<' || first == '>') && ch == first)
        {
            next = fgetc(src);

            if (next == '=')
            {
                add_char(token, next);
            }
            else if (next != EOF)
            {
                ungetc(next, src);
            }
        }

        token->type = TOKEN_OPERATOR;
        return 1;
    }

    if (ch != EOF)
    {
        ungetc(ch, src);
    }

    token->type = TOKEN_OPERATOR;
    return 1;
}

/* Read a preprocessor directive */
static int read_preprocessor(FILE *src, Token *token)
{
    int ch;

    add_char(token, '#');

    while ((ch = fgetc(src)) != EOF && ch != '\n')
    {
        add_char(token, ch);
    }

    if (ch == '\n')
    {
        ungetc(ch, src);
    }

    token->type = TOKEN_PREPROCESSOR;
    return 1;
}

/* Return the next token from the source file */
int get_next_token(FILE *src, Token *token)
{
    int ch;

    if (src == NULL || token == NULL)
    {
        return 0;
    }

    token->value[0] = '\0';

    ch = fgetc(src);

    if (ch == EOF)
    {
        return 0;
    }

    /* Preserve spaces, tabs and newlines */
    if (isspace((unsigned char)ch))
    {
        do
        {
            add_char(token, ch);
            ch = fgetc(src);
        }
        while (ch != EOF && isspace((unsigned char)ch));

        if (ch != EOF)
        {
            ungetc(ch, src);
        }

        token->type = TOKEN_WHITESPACE;
        return 1;
    }

    if (ch == '#')
    {
        return read_preprocessor(src, token);
    }

    if (is_identifier_start(ch))
    {
        return read_identifier(src, token, ch);
    }

    if (isdigit((unsigned char)ch))
    {
        return read_number(src, token, ch);
    }

    if (ch == '"' || ch == '\'')
    {
        return read_quoted(src, token, ch);
    }

    if (ch == '/')
    {
        return read_slash_token(src, token);
    }

    if (strchr("=<>!+-*&|%^~?:", ch) != NULL)
    {
        return read_operator(src, token, ch);
    }

    add_char(token, ch);

    if (strchr("(){}[];,.", ch) != NULL)
    {
        token->type = TOKEN_PUNCTUATOR;
    }
    else
    {
        token->type = TOKEN_UNKNOWN;
    }

    return 1;
}
