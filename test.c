#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define bool int
#define false 0
#define true 1

typedef struct Text
{
    char **rows;
    int number_of_lines;
} Text;

Text createText(FILE *in, int col_per_pg, int lines_per_cl, int col_w, int sib)
{
    char c;                                                          // current character
    char *line_buffer = (char *)calloc(col_w + 1, sizeof(char) * 4); // current string buffer
    char *word_buffer = (char *)calloc(col_w + 1, sizeof(char) * 4); // current word buffer

    int line_length = 0; // how many char now in line
    int line_size = 0;   // how many bytes in line
    int word_size = 0;   // how many bytes does the current word weight
    int word_length = 0; // how long is the current word

    Text text;                                                            // declaration of text struct
    int line_offset = 0;                                                  // current line counter
    text.rows = (char **)calloc((line_offset + 1), sizeof(char *));       // allocating space for+ text, it has lines_per_cl pointer to strings
    text.rows[line_offset] = (char *)calloc(col_w + 1, sizeof(char) * 4); // allocating space for a row

    // I read the character and trace length and int line_offset = 0;
    while ((c = fgetc(in)) != EOF)
    {
        if ((unsigned char)c < 128 && c != '\r' && c != '\n')
        {
            // ASCII character
            word_buffer[word_size] = c;
            word_length++;
            word_size++;
        }
        else if ((unsigned char)c > 127)
        {
            // UTF-8 character
            if ((c & 0xC0) != 0x80)
            { // then it is the first byte
                word_buffer[word_size] = c;
                word_length++;
                word_size++;
            }
            else
            { // then it is NOT the first byte
                word_buffer[word_size] = c;
                word_size++;
            }
        }
        // what to do if i found a space or the last word of a paragraph
        if (c == ' ' || (c == '\n' && word_size > 0))
        {
            // Space character
            // if i have enough space to add the word to the current line (considering it without the last space)
            if (word_length + line_length - 1 <= col_w)
            {
                memcpy(line_buffer + line_size, word_buffer, word_size);
                line_size += word_size;
                line_length += word_length;
                // clean buffer and its counters
                free(word_buffer);
                word_buffer = (char *)calloc(col_w + 1, sizeof(char) * 4);
                word_length = 0;
                word_size = 0;
            }
            // if i do not have enough space
            else
            {
                // allocating memory for next string
                line_offset++;
                text.rows = (char **)realloc(text.rows, (line_offset + 1) * sizeof(char *));
                text.rows[line_offset] = (char *)calloc((col_w + 1), sizeof(char) * 4);
                // saving current line
                memcpy(text.rows[line_offset - 1], line_buffer, line_size);
                // reset the line buffer
                free(line_buffer);
                line_buffer = (char *)calloc((col_w + 1) * 4, sizeof(char));
                line_length = word_length;
                line_size = word_size;
                // saving current word in line buffer
                memcpy(line_buffer, word_buffer, line_size);
                // saving current word in the struct
                memcpy(text.rows[line_offset], word_buffer, word_size);
                //("%s\n", text.rows[line_offset - 1]);
                word_length = 0;
                word_size = 0;
            }
        }
        // managing the first newline
        if (c == '\n' && line_size > 0)
        {
            // printf("%s\n", text.rows[line_offset]);
            memcpy(text.rows[line_offset], line_buffer, line_size);
            line_size = 0;
        }
        // managing the newline between paragraph
        else if (c == '\n' && line_size == 0)
        {
            /*line_offset++;
            text.rows = (char **)realloc(text.rows, (line_offset + 1) * sizeof(char *));
            text.rows[line_offset] = (char *)calloc((col_w+1)*4, sizeof(char));
            free(word_buffer);
            word_buffer = calloc((col_w + 1) * 4, sizeof(char));
            word_buffer[word_size] = c;
            memcpy(text.rows[line_offset - 1], word_buffer, word_size);
            free(word_buffer);
            word_buffer = (char *)calloc((col_w + 1), sizeof(char) * 4);
            word_size = 0;
            free(line_buffer);
            line_buffer = (char *)calloc((col_w + 1), sizeof(char) * 4);
            line_size = 0;
            line_length = 0;
            */
            line_offset += 2;
            text.rows = (char **)realloc(text.rows, (line_offset + 2) * sizeof(char *));
            text.rows[line_offset - 1] = (char *)calloc((col_w + 1) * 4, sizeof(char));
            text.rows[line_offset] = (char *)calloc((col_w + 1) * 4, sizeof(char));
            free(word_buffer);
            word_buffer = calloc((col_w + 1), sizeof(char) * 4);
            free(line_buffer);
            line_buffer = (char *)calloc((col_w + 1), sizeof(char) * 4);
            line_size = 0;
            line_length = 0;
        }
    }
    text.number_of_lines = line_offset;
    return text;
}

int main()
{
    FILE *in = fopen("SOII Progetto 2022-23 - Esempio Input.txt", "r");
    if (in == NULL)
    {
        printf("Error occured while opening input file\n");
        exit(1);
    }
    FILE *out = fopen("output.txt", "w");
    if (out == NULL)
    {
        printf("Error occured while opening output file");
        exit(1);
    }
    int columns_per_page = 3;
    int lines_per_columns = 700;
    int column_width = 23;
    int spaces_between_columns = 3;
    Text text;
    text = createText(in, columns_per_page, lines_per_columns, column_width, spaces_between_columns);

    for (int i = 0; i <= text.number_of_lines; i++)
    {
        printf("%s\n", text.rows[i]);
    }

    return 0;
}