User-Guide

1 - Files Included and description

This program use main.c, functions.c and struct.h to make a journal-like text from a txt file given as input.
main.c calls functions included in functions.c to analyze the given input and the parameters given by the user with the shell.
functions.c contains functions that manage the text, justify the lines and create journal-like version of the input. A version of strlen that counts character instaed of bytes in a string is also included.
struct.h contains a simple definition of the Text struct. The Text struct is NECESSARY to have this program working.

2 - Text requirements

Every text given as input should have lines divided by newline that the programs will read as paragraph. e.g.

	This is a text that the user should use as example.
	
	So to be satisfied by the result.

	Every line should be divided by a white new line.

	End.

Every paragraph must end with a newline, it is MANDATORY that even the last paragraph end with a newline.

3 - How to use

Arguments given must be in this order: input-path, columns per page, lines per column, column width, distance between column.
