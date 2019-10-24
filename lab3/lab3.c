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
#include <dirent.h>
#include <unistd.h>
/*---------------------------------------------------------------------------*/
void lfcat();
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
			if(strcmp(token, "lfcat\n") == 0){
					lfcat();
				}
			else {
				printf("Error: Unrecognised command!\n");
			}
			token = strtok(NULL, " ");
		}		
		/* If the user entered <exit> then exit the loop */

	}
	/*Free the allocated memory*/
	free(input);
	return 1;
}

void lfcat(){
	
	FILE *out_file = fopen("output.txt", "w");
	
	FILE *in_file;
	char *buff = (char *)malloc(sizeof(char) * 1024);
	char *pwd = (char *)malloc(sizeof(char) * 1024);
	ssize_t line_size = 0;
	size_t buff_size = 1024;
	int file_cnt = 1;
	DIR *dir;
	struct dirent *dp;

	getcwd(pwd, 1024);
	dir = opendir(pwd);
	while(dir){
		if((dp = readdir(dir)) != NULL) {
			if(strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..") && strcmp(dp->d_name, "output.txt") &&
					strcmp(dp->d_name, "lab3.c") && strcmp(dp->d_name, "a.out")) {
				
				in_file = fopen(dp->d_name, "r");
				fprintf(out_file, "File %d: %s\n", file_cnt, dp->d_name);
				line_size = getline(&buff, &buff_size, in_file);
				while(line_size >= 0) {
					fprintf(out_file, "%s\n", buff);
					line_size = getline(&buff, &buff_size, in_file);
				}
				fclose(in_file);
				fprintf(out_file, "------------------------------------------------------------------------------------------");
				file_cnt++;
			}
			
			else {
				//do nothing		
			}

				


		}
		else{
			closedir(dir);
			break;
		}
	}



	fclose(out_file);
	free(pwd);
	free(buff);
	
}
/*-----------------------------Program End-----------------------------------*/
