
#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#define TOKEN_SIZE 256

typedef enum
{
    TOKEN_WHITESPACE,
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_CHARACTER,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATOR,
    TOKEN_COMMENT,
    TOKEN_PREPROCESSOR,
    TOKEN_UNKNOWN
} TokenType;

typedef struct
{
    char value[TOKEN_SIZE];
    TokenType type;
} Token;

int get_next_token(FILE *src, Token *token);

#endif
