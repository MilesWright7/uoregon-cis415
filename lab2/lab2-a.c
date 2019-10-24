/*
* Description: <write a brief description of your lab>
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
int main() {
	setbuf(stdout, NULL);
	
	/* Main Function Variables */
	size_t size = 100;
	char *input;
	char *token;
	int token_count;

	/* Allocate memory for the input buffer. */
	
	input = (char *)malloc(sizeof(char) * size);
	/*main run loop*/
	while(1) {
		/* Print >>> then get the input string */
		printf(">>>");
		getline(&input, &size , stdin);	
		/* Tokenize the input string */

		token = strtok(input, " ");
		
		/* Display each token */
		
		if(strcmp(token, "exit\n") == 0){
			printf("\n");
			break;
		}				

		token_count = 0;
		while(token != NULL && strcmp(token, "\n")) {
			printf("\nT%d: %s", token_count, token);
			token_count ++;	
			token = strtok(NULL, " ");
		}		
		/* If the user entered <exit> then exit the loop */

	}
	/*Free the allocated memory*/
	free(input);
	return 1;
}
/*-----------------------------Program End-----------------------------------*/
