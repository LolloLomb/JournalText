// a Text type is a struct made of array of strings and a counter for the number of strings in it

#include <stdio.h>

typedef struct Text
{
    char **rows;
    int number_of_lines;
} Text;

int realLen(char *string);
Text createText(FILE *in, int col_w);
void justifyText(Text text, int col_w);
Text journalText(Text text, int lines_per_column, int sib, int col_w, int col_per_page);
void printJournal(FILE *out, Text journal, int lpc);
int check(int cpp, int cw, int sib, int lpc);