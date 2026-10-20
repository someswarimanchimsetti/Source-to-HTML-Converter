
#include <stdio.h>
#include "lexer.h"

int main(void)
{
    FILE *src = fopen("sample.c", "r");
    Token token;

    if (src == NULL)
    {
        perror("sample.c");
        return 1;
    }

    while (get_next_token(src, &token))
    {
        printf("%-20s : ", token.value);

        switch (token.type)
        {
        case TOKEN_WHITESPACE:
            printf("WHITESPACE");
            break;

        case TOKEN_KEYWORD:
            printf("KEYWORD");
            break;

        case TOKEN_IDENTIFIER:
            printf("IDENTIFIER");
            break;

        case TOKEN_NUMBER:
            printf("NUMBER");
            break;

        case TOKEN_STRING:
            printf("STRING");
            break;

        case TOKEN_CHARACTER:
            printf("CHARACTER");
            break;

        case TOKEN_OPERATOR:
            printf("OPERATOR");
            break;

        case TOKEN_PUNCTUATOR:
            printf("PUNCTUATOR");
            break;

        case TOKEN_COMMENT:
            printf("COMMENT");
            break;

        case TOKEN_PREPROCESSOR:
            printf("PREPROCESSOR");
            break;

        default:
            printf("UNKNOWN");
            break;
        }

        printf("\n");
    }

    fclose(src);
    return 0;
}
