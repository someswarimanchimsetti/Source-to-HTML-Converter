#include <stdio.h>
#include <string.h>

#define MAX_SIZE 100

/* Function to check a keyword */
int is_keyword(const char *word)
{
    if (strcmp(word, "int") == 0 ||
        strcmp(word, "return") == 0 ||
        strcmp(word, "if") == 0)
    {
        return 1;
    }

    return 0;
}

int main(void)
{
    int number = 25;
    float price = 12.5;
    char letter = 'A';
    char name[] = "Hello World";

    // Display sample values
    printf("Number: %d\n", number);
    printf("Price: %.2f\n", price);
    printf("Letter: %c\n", letter);
    printf("Name: %s\n", name);

    if (number > 10)
    {
        printf("Number is greater than 10\n");
    }

    /*
     * This is a
     * multiline comment.
     */

    return 0;
}