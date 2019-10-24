/*
* Description: We read and tokenize text from a file then write tokens to a file
*
* Author: Miles Wright
*
* Date: 10/9/2019
*
* Notes: 
* 1. <add notes we should consider when grading>
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*---------------------------------------------------------------------------*/

/*-----------------------------Program Main----------------------------------*/
int main(int argc, char* argv[]) {
	FILE *f_in = fopen(argv[1], "r");
	FILE *f_out = fopen("output_file.txt", "w");
	setbuf(stdout, NULL);
	
	/* Main Function Variables */
	size_t size = 100;
	char *input;
	char *token;
	int token_count;

	/* Allocate memory for the input buffer. */
	
	input = (char *)malloc(sizeof(char) * size);
	/*main run loop*/
	while(fgets(input, size, f_in) != NULL) {
		/* Tokenize the input string */
		printf(input);
		token = strtok(input, " ");	
		/* Display each token */
		
		token_count = 0;
		while(token != NULL && strcmp(token, "\n")) {
			fprintf(f_out, "\nT%d: %s", token_count, token);
			token_count ++;	
			token = strtok(NULL, " ");
		}		
		/* If the user entered <exit> then exit the loop */

	}
	/*Free the allocated memory*/
	free(input);
	fclose(f_in);
	fclose(f_out);
	return 1;
}
/*-----------------------------Program End-----------------------------------*/
