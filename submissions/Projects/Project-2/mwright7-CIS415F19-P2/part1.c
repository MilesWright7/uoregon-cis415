/*
 * Author Miles Wright
 *
 * Project 2 for cis 415
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/*function declarations*/

/*main and functions*/

int main(int argc, char *argv[]){
	
	FILE *inFile;
	if(argc == 1){
		perror("Please specify an input file.\n");
		exit(EXIT_FAILURE);
	}
	else if(argc > 2){
		perror("Too many arguments.\n");
		exit(EXIT_FAILURE);
	}
	else{
		// argc == 2
		inFile = fopen(argv[1], "r");
		if(inFile == NULL){
			perror("Could not open file.\n");
			exit(EXIT_FAILURE);
		}
	}

	pid_t pid[10];
	char *arguments[5];
	size_t size = 100;
	char *input = (char *)malloc(sizeof(char) * size);
	char *token = (char *)malloc(sizeof(char) * size);
	int i, argCount, wstatus, rstatus;
	int programCount = 0;
	int ammount = 1;
	pid_t ended;

	

	// read input file
	ammount = getline(&input, &size, inFile);
	while(ammount > 0){
		i = 0;
		while(input[i] != '\0'){
			if(input[i] == '\n'){
				input[i] = '\0';
			}
			i++;
		}
		

		token = strtok(input, " ");
		argCount = 0;
		while(token != NULL){
			arguments[argCount] = token;
			argCount++;
			token = strtok(NULL, " ");
		}
		arguments[argCount] = NULL;
		
		// call exec and run programs
		pid[programCount] = fork();
		if(pid[programCount] == 0){
			printf("Starting child program %s with pid %d\n", arguments[0], getpid());
			rstatus = execvp(arguments[0], arguments);
			if(rstatus == -1){
				printf("Error calling program %s in child with pid %d\n", arguments[0], getpid());
			}
			exit(EXIT_SUCCESS);
		}
		else{
			// in parent continuing through loops
			programCount++;
			ammount = getline(&input, &size, inFile);
		}
	}
	
	// wait for all to finish
	printf("Parent started all children down the road.\n");
	for(i = 0; i < programCount; i++){
		ended = waitpid(pid[i], &wstatus, WUNTRACED | WCONTINUED);
	   	printf("Child %d finished executing.\n", ended);
	}
	printf("All children finished.\n");

	free(input);
	free(token);
}
