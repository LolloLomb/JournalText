#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

#define bool int
#define false 0
#define true 1

// realLen is a function that counts the real number of character in a string, not the bytes as strelen do
// input : a string
// output : string length as number of character
int realLen(char *string)
{
    int len = 0;
    int i = 0;
    while (string[i] != '\0')
    {
        if ((unsigned char)string[i] < 128)
        {
            // ASCII character
            len++;
            i++;
        }
        else if ((unsigned char)string[i] > 127)
        {
            // UTF-8 character
            if ((string[i] & 0xC0) != 0x80)
            { // then it is the first byte
                len++;
                i++;
            }
            else
            { // then it is NOT the first byte
                i++;
            }
        }
    }
    return len;
}

// createText is a function that create a Text struct where every line is made at most of col_width characters
// there will be not truncated words
// input : a File input stream, column width
// output : a Text struct
Text createText(FILE *in, int col_w)
{
    // buffer for current character
    char c = ' ';
    bool newlineFlag = false;
    // prev_c is used as a flag so i count every block of sequential spaces as one space
    char prev_c = '\0';

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

        if (c != '\n' && c != '\r')
        {
            newlineFlag = 0;
        }

        if (c == ' ' && prev_c == ' ')
        {
            // manage block of sequential spaces jumping to next character without going on with this iteration
            continue;
        }

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
            if (word_length + line_length <= col_w)
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
                memcpy(text.rows[line_offset - 1], line_buffer, line_size - 1);
                // reset the line buffer
                free(line_buffer);
                line_buffer = (char *)calloc((col_w + 1) * 4, sizeof(char));
                line_length = word_length;
                line_size = word_size;
                // saving current word in line buffer
                memcpy(line_buffer, word_buffer, line_size);
                // saving current word in the struct
                memcpy(text.rows[line_offset], word_buffer, word_size);
                word_length = 0;
                word_size = 0;

                // CONTROL IF WORD IS TOO LONG FOR THE COL_W
                if (realLen(text.rows[line_offset]) > col_w)
                {
                    // return a text with a NULL pointer as first string
                    text.rows[0] = NULL;
                    return text;
                }
            }
        }
        // managing the first newline
        if (c == '\n' && line_size > 0 && newlineFlag == 0)
        {
            memcpy(text.rows[line_offset], line_buffer, line_size);
            line_size = 0;
        }

        // managing the newline between paragraph
        else if (c == '\n' && line_size == 0 && newlineFlag == 0)
        {
            newlineFlag = 1;
            line_offset += 2;
            text.rows = (char **)realloc(text.rows, (line_offset + 2) * sizeof(char *));
            text.rows[line_offset - 1] = (char *)calloc((col_w + 1) * 4, sizeof(char));
            for (int i = 0; i < col_w; i++)
            {
                text.rows[line_offset - 1][i] = ' ';
            }
            text.rows[line_offset] = (char *)calloc((col_w + 1) * 4, sizeof(char));
            free(word_buffer);
            word_buffer = calloc((col_w + 1), sizeof(char) * 4);
            free(line_buffer);
            line_buffer = (char *)calloc((col_w + 1), sizeof(char) * 4);
            line_size = 0;
            line_length = 0;
        }
        prev_c = c;
    }
    memcpy(text.rows[line_offset], line_buffer, line_size);
    if (line_length + word_length > col_w)
    {
        text.rows = (char **)realloc(text.rows, (line_offset + 1) * sizeof(char *));
        text.rows[line_offset + 1] = (char *)calloc((col_w + 1) * 4, sizeof(char));
        line_offset++;
        line_size = 0;
    }
    memcpy(text.rows[line_offset] + line_size, word_buffer, word_size);
    text.number_of_lines = line_offset + 1;
    // initialize last line empty
    return text;
}

// justifyText is a function that justify every line of a Text struct distributing spaces
// input : Text struct, column width
// output : no output
void justifyText(Text text, int col_w)
{
    // i know the total number of lines
    int line_offset = 0;
    int total_spaces = 0;
    int total_words = 0;
    // leftover is used to manage asymmetric spaces in a line
    int leftover = 0;
    // distribute is used to know the minimum spaces between every word in a line
    int distribute = 0;
    // for every line in a text
    while (line_offset < text.number_of_lines)
    {
        int i = 0;
        char *string = calloc((col_w + 1), sizeof(char) * 4);

        while (i <= strlen(text.rows[line_offset]))
        {
            // count the spaces
            // number of spaces to be redistributed is #32 + #0
            if (text.rows[line_offset][i] == ' ' || text.rows[line_offset][i] == '\0')
            {
                total_spaces++;
                total_words++;
            }
            i++;
        }
        // AT THIS POINT IVE GOT TOTAL SPACES
        // there's a need to create a new line and to overwrite the old one
        // at this point i know the spaces in the current line
        if (total_words != 1)
        {
            total_spaces += col_w - realLen(text.rows[line_offset]) - 1;
            leftover = total_spaces % (total_words - 1);
            distribute = total_spaces / (total_words - 1);
        }
        else
        {
            leftover = 0;
            distribute = col_w - realLen(text.rows[line_offset]);
        }
        // jump is a counter that increase its value during the construction of the justified line
        int jump = 0;

        for (i = 0; i < strlen(text.rows[line_offset]); i++)
        {

            if (text.rows[line_offset][i] != ' ')
            {
                string[jump] = text.rows[line_offset][i];
                jump++;
            }
            else
            {
                if (leftover > 0)
                { // inside if im not saving the last word
                    for (int k = 0; k < distribute + 1; k++)
                    {
                        string[jump] = ' ';
                        jump++;
                    }
                    leftover--;
                }
                else if (leftover == 0)
                {
                    for (int k = 0; k < distribute; k++)
                    {
                        string[jump] = ' ';
                        jump++;
                    }
                }
            }
        }
        // if a line is made of a single word, add enough spaces
        if (total_words == 1)
        {
            while (distribute > 0)
            {
                string[jump] = ' ';
                distribute--;
                jump++;
            }
        }

        line_offset++;
        total_spaces = 0;
        total_words = 0;
        memcpy(text.rows[line_offset - 1], string, col_w * 4);
        free(string);
    }
}

// journalText is a function that create a Text struct where every string is made of every first string of every column
// input : original Text struct, lines per column, spaces between columns, column width, column per page
// output : new Text struct made as a journal
Text journalText(Text text, int lines_per_column, int sib, int col_w, int col_per_page)
{
    Text journal;
    // i need to know how many pages i need
    int lines_per_page = lines_per_column * col_per_page;
    int number_of_pages = text.number_of_lines / lines_per_page;

    if (text.number_of_lines > number_of_pages * lines_per_page)
    {
        number_of_pages++; // i need one more
    }
    // at this point i know how many pages i need
    // i need to fill original text with spaces to avoid segmentation fault
    // how many spaces i need?
    int counter = text.number_of_lines;
    text.rows = (char **)realloc(text.rows, sizeof(char *) * (number_of_pages * lines_per_page));
    // now every new string that will be filled with spaces is addressable
    while (counter < number_of_pages * lines_per_page)
    {
        // fill with spaces every new line until every column is filled
        text.rows[counter] = calloc(col_w + 1, sizeof(char) * 4);
        for (int i = 0; i < col_w; i++)
        {
            text.rows[counter][i] = ' ';
        }
        text.rows[counter][col_w] = '\0';
        counter++;
    }
    text.number_of_lines = number_of_pages * lines_per_page;
    // now i got my justified single column ready to be on a page
    // i prepare all the pointers to big string
    journal.rows = (char **)calloc(number_of_pages * lines_per_column, sizeof(char *));
    int current_string = 0;
    int current_char = 0;
    int current_page = 0;
    char *s;
    // i need to manage every page
    while (current_page < number_of_pages)
    {
        // i need to manage every string
        // every i-string is made of every i-string of every column and spaces in between
        // so i will have (lines per column) number of strings per page
        while (current_string < lines_per_column)
        {
            journal.rows[current_string + current_page * lines_per_column] = calloc(col_w * col_per_page + sib * (col_per_page - 1) + 1, sizeof(char) * 4);
            char *s = calloc(col_w * col_per_page + sib * (col_per_page - 1) + 1, sizeof(char) * 4);
            for (int i = 0; i < col_per_page; i++)
            {
                memcpy(s + current_char, text.rows[i * lines_per_column + current_string + current_page * lines_per_page], strlen(text.rows[i * lines_per_column + current_string + current_page * lines_per_page]));
                current_char += strlen(text.rows[i * lines_per_column + current_string + current_page * lines_per_page]);
                for (int j = 0; j < sib; j++)
                {
                    s[j + current_char] = ' ';
                }
                current_char += sib;
            }
            memcpy(journal.rows[current_string + current_page * lines_per_column], s, strlen(s));
            free(s);
            current_string++;
            current_char = 0;
        }
        current_page++;
        current_string = 0;
    }
    // set the number of lines so the ones who have access to journal know its size
    journal.number_of_lines = number_of_pages * lines_per_column;
    return journal;
}

// printfJournal is a function that prints a text in a file
// input : pointer to output file, a Text struct, line per column
// output : no output
void printJournal(FILE *out, Text journal, int lpc)
{
    int line_counter = 0;
    int page_counter = 0;
    int counter = 0;
    int total_pages = journal.number_of_lines / lpc;

    while (page_counter < total_pages)
    {
        while (line_counter < lpc)
        {
            fprintf(out, "%s\n", journal.rows[counter]);
            counter++;
            line_counter++;
        }
        // printing % % % so i know when a page is over
        fprintf(out, "\n %%%%%% \n\n");
        page_counter++;
        line_counter = 0;
    }
}

// check is a function that checks if the input satisfies the requirements for journal creation
// input : column per page, column width, spaces between columns, lines per column
// output : 0 if everything's ok, else -1
int check(int cpp, int cw, int sib, int lpc)
{
    if (cpp < 1)
    {
        printf("Columns per page must be at least 1!");
        return -1;
    }
    if (cw < 1)
    {
        printf("Column width must be at least 1");
        return -1;
    }
    if (sib < 1)
    {
        printf("For a readable result spaces between lines must be at least 1");
        return -1;
    }
    if (lpc < 1)
    {
        printf("Lines per column must be at least 1");
        return -1;
    }
    return 0;
}