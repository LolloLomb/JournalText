#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "header.h"

// the main function manage the input and checks if it satisfies the requirements

void usage()
{
    printf("\nUsage: journal <input_path> <column_per_page> <lines_per_column> <column_width> <spaces_between_columns>\n\n");
    exit(2);
}

int main(int argc, char *argv[])
{
    int option;
    while ((option = getopt(argc, argv, "h")) != -1)
    {
        switch (option)
        {
        default:
            usage();
        }
    }

    FILE *in = fopen(argv[1], "r");
    if (in == NULL)
    {
        printf("Error occured while opening input file\n");
        exit(1);
    }
    FILE *out = fopen("journal.txt", "w");
    if (out == NULL)
    {
        printf("Error occured while opening output file");
        exit(1);
    }

    int columns_per_page = atoi(argv[2]);
    int lines_per_column = atoi(argv[3]);
    int column_width = atoi(argv[4]);
    int spaces_between_columns = atoi(argv[5]);

    if (check(columns_per_page, column_width, spaces_between_columns, lines_per_column) == -1)
    {
        exit(1);
    }

    Text text;

    text = createText(in, column_width);
    if (text.rows[0] == NULL)
    {
        printf("I've found a word that is too long for the column width desired\n");
        return -1;
    }

    justifyText(text, column_width);
    text = journalText(text, lines_per_column, spaces_between_columns, column_width, columns_per_page);
    printJournal(out, text, lines_per_column);

    return 0;
}