
/*
* Description: Basic shell implementation. Shoud be able to preform the basic 
* unix commands ls, pwd, mkdir, cd, cp,. mv, rm, and cat.
*
* Author: Miles Wright
*
* Date: 10/22/2019
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
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "command.h"
/*---------------------------------------------------------------------------*/
void printError(int errno, char *extra);
int isCommand(char *string);
/*-----------------------------Program Main----------------------------------*/
int main(int argc, char *argv[]) {
	int mode; // 1 = shell | 2 = file

	FILE *in_file; // infile must be FILE * for getline

	/*Command Line error handling*/
	char *error = "Please use -command for shell mode or supply a batch file with -f <filename>\n";
	if(argc == 2){
		if(strcmp("-command", argv[1]) != 0){
		write(STDOUT_FILENO, error, strlen(error));
		exit(EXIT_FAILURE);
		}
		else{
			mode = 1;
		}
	}
	else if(argc == 3){
		if(strcmp("-f" , argv[1]) != 0){
		write(STDOUT_FILENO, error, strlen(error));
		exit(EXIT_FAILURE);
		}
		else{
			in_file = fopen(argv[2], "r");
			if(in_file == NULL) {
				write(STDOUT_FILENO, "Incorrect batchfile name!\n", 26);
				exit(EXIT_FAILURE);
			}
			else{
				mode = 2;
				// ensure that output.txt is empty file to start
				deleteFile("output.txt");
				int new_out = open("output.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRWXU | S_IRWXG);
				dup2(new_out, STDOUT_FILENO);
				close(new_out);
			}
		}
	}
	else {
	 	// argc must be 1 or more than 3 so cant be -command or -f <filename>
		write(STDOUT_FILENO, error, strlen(error));
		exit(EXIT_FAILURE);
	}


	
	/* Main Function Variables */
	size_t size = 100;
	int ammount;
	char *input;
	char *token;
	int token_count, token_index;
	char **token_array;
	int running = 1;
	/* Allocate memory for the input buffer. */
	
	input = (char *)malloc(sizeof(char) * size);
	token_array = (char **)malloc(sizeof(char *) * 100);
	for(int i =0; i < 100; i++){
		token_array[i] = malloc(sizeof(char) * 100);
	}

	/*main run loop*/
	while(running) {
		/* Print >>> then get the input string */
		if(mode == 1){
			write(STDOUT_FILENO, ">>>", 3);
			getline(&input, &size , stdin);	
		}
	
		if(mode == 2){
			ammount = getline(&input, &size, in_file);
			if(ammount <= 0){
				
				break;
			}
		}

		/* Tokenize the input string */
		token_count = 0;
		token = strtok(input, " ");
		while(token != NULL) {
			strcpy(token_array[token_count], token);
			token_count++;
			token = strtok(NULL, " ");

		}
		strcpy(token_array[token_count], "");
		

		token_index = 0;
		while(strcmp(token_array[token_index], "") != 0 && strcmp(token_array[token_index], "\n") != 0) {
			if((strcmp(token_array[token_index], "ls\n") == 0) || (strcmp(token_array[token_index], "ls") == 0)) {
				token_index++;
				// correct use
				if(strcmp(token_array[token_index], "") == 0){
					listDir();
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					listDir();
					token_index++;
				}
				// too many commands 
				else if(isCommand(token_array[token_index])){
					printError(1, token_array[token_index]);
					break;
				}	 
				else{
					printError(3, token_array[token_index - 1]);
					break;
				}
			}
	
			else if((strcmp(token_array[token_index], "pwd\n") == 0) || (strcmp(token_array[token_index], "pwd") == 0)) {
				token_index++;
				// correct use
				if(strcmp(token_array[token_index], "") == 0){
					showCurrentDir();
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					showCurrentDir();
					token_index++;
				}
				// too many commands
				else if(isCommand(token_array[token_index])){
					printError(1, token_array[token_index]);
					break;
				}
				else{
					printError(3, token_array[token_index - 1]);
					break;
				}
			}
			
			else if((strcmp(token_array[token_index], "mkdir\n") == 0) || (strcmp(token_array[token_index], "mkdir") == 0)) {
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					makeDir(token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					makeDir(token_array[token_index - 1]);
					token_index++;
				}
				else{
					printError(3, token_array[token_index - 2]);
					break;
				}
			}
		
			else if((strcmp(token_array[token_index], "cd\n") == 0) || (strcmp(token_array[token_index], "cd") == 0)) {
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					changeDir(token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					changeDir(token_array[token_index - 1]);
					token_index++;
				}
				else{
					printError(3, token_array[token_index - 2]);
					break;
				}
			}
			
			else if((strcmp(token_array[token_index], "cp\n") == 0) || (strcmp(token_array[token_index], "cp") == 0)) {
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					copyFile(token_array[token_index - 2], token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					copyFile(token_array[token_index - 2], token_array[token_index - 1]);
					token_index++;
				}
				else{
					printError(3, token_array[token_index - 2]);
					break;
				}
			}
			
			else if((strcmp(token_array[token_index], "mv\n") == 0) || (strcmp(token_array[token_index], "mv") == 0)) {
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					moveFile(token_array[token_index - 2], token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					moveFile(token_array[token_index - 2], token_array[token_index - 1]);
					token_index++;
				}
				else{
					printError(3, token_array[token_index - 2]);
					break;
				}
			}
			
			else if((strcmp(token_array[token_index], "rm\n") == 0) || (strcmp(token_array[token_index], "rm") == 0)) {
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					deleteFile(token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					deleteFile(token_array[token_index - 1]);
					token_index++;
				}
				else{
					printError(3, token_array[token_index - 2]);
					break;
				}
			}
		
			else if((strcmp(token_array[token_index], "cat\n") == 0) || (strcmp(token_array[token_index], "cat") == 0)) {
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					printError(3, token_array[token_index - 1]);
					break;
				}
				token_index++;
				if(strcmp(token_array[token_index], "") == 0){
					displayFile(token_array[token_index - 1]);
					break;
				}
				else if(strcmp(token_array[token_index], ";") == 0){
					displayFile(token_array[token_index - 1]);
					token_index++;
				}
				else{
					printError(3, token_array[token_index - 2]);
					break;
				}
			}
			
			else if((strcmp(token_array[token_index], "exit\n") == 0) || (strcmp(token_array[token_index], "exit") == 0)){
				write(STDOUT_FILENO, "\n", 1);
				running = 0;
				break;
			}
			else {
				printError(2, token_array[token_index]);
				break;
			}
		}		

	}
	/*Free the allocated memory*/
	if(mode == 2){
		fclose(in_file);
	}
	for(int i = 0; i < 100; i++){
		free(token_array[i]);
	}
	free(token_array);
	free(input);
	fflush(stdout);
	return 1;
}


void printError(int errno, char *extra){
	if(errno == 1){
	    char errorMessage[100] = "Error! Incorrect syntax. No control code found.\n\0";
		write(STDOUT_FILENO, errorMessage, strlen(errorMessage));
	}
	else if(errno == 2){
		char errorMessage[100] = "Error! Unrecognised command: \0";
		strcat(errorMessage, extra);
		strcat(errorMessage, "\n");
		write(STDOUT_FILENO, errorMessage, strlen(errorMessage));
	}
	else if(errno == 3){
		char errorMessage[100] = "Error! Unsupported parameters for command: \0";
		strcat(errorMessage, extra);
		strcat(errorMessage, "\n");
		write(STDOUT_FILENO, errorMessage, strlen(errorMessage));
	}
}

int isCommand(char *string){

	if((strcmp(string, "ls\n") == 0) || (strcmp(string, "ls") == 0)) {
		return 1;
	}	   
	else if((strcmp(string, "pwd\n") == 0) || (strcmp(string, "pwd") == 0)) {
		return 1;
	}
	else if((strcmp(string, "mkdir\n") == 0) || (strcmp(string, "mkdir") == 0)) {
		return 1;
	}
	else if((strcmp(string, "cd\n") == 0) || (strcmp(string, "cd") == 0)) {
		return 1;
	}
	else if((strcmp(string, "cp\n") == 0) || (strcmp(string, "cp") == 0)) {
		return 1;
	}
	else if((strcmp(string, "mv\n") == 0) || (strcmp(string, "mv") == 0)) {
		return 1;
	}
	else if((strcmp(string, "rm\n") == 0) || (strcmp(string, "rm") == 0)) {
		return 1;
	}
	else if((strcmp(string, "cat\n") == 0) || (strcmp(string, "cat") == 0)) {
		return 1;
	}
	else if((strcmp(string, "exit\n") == 0) || (strcmp(string, "exit") == 0)){
		return 1;
	}
	return 0;
}	

