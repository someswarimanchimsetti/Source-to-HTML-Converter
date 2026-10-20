
#ifndef HTML_H
#define HTML_H

#include <stdio.h>

int is_keyword(char *word);
int is_macro(char *word);
void write_word(FILE *dest, char *word);
void write_number(FILE *dest, char *number);
void write_character(FILE *src, FILE *dest);
void write_special_char(FILE *dest, int ch);
void write_preprocessor(FILE *src, FILE *dest);

#endif
