/*
 * Author Miles Wright
 *
 * Project 2 for cis 415
 *
 * part 3
 *
 * using signals and alarms to control execution order etc.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/*function declarations*/

pid_t pid[10];
int currentProgram, programCount;
void signal_handler(int signal);
void signal_init();
struct sigaction sigact;
void send_signal(pid_t *pool, int size, int sig);
int some_running(pid_t *pool, int size);

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

	char *arguments[5];
	size_t size = 100;
	char *input = (char *)malloc(sizeof(char) * size);
	char *token = (char *)malloc(sizeof(char) * size);
	int i, argCount, wstatus, rstatus, sig, cycle;
	programCount = 0;
	int ammount = 1;
	sigset_t set;

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
			signal_init();
			sigemptyset(&set);
			sigaddset(&set, SIGUSR1);
			printf("Child %d waiting for signal to start.\n", getpid());
			sigwait(&set, &sig);
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
	sleep(2);
	printf("Parent created all children. Sending start signal.\n");
	signal_init();
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	currentProgram = 0;
	send_signal(pid, programCount, SIGUSR1);
	sleep(1);
	
	for(i = 0; i < programCount; i ++){
		if(i == currentProgram){
			// do nothing
		}
		else{
			kill(pid[i], SIGSTOP);
		}
	}
	// all programs stopped except for first one
	printf("Starting process %d\n", pid[0]);
	alarm(3);
	sigwait(&set, &sig);
	while(some_running(pid, programCount)){
		if(cycle == 0){
			printf("Stopping process %d\n", pid[currentProgram % programCount]);
			kill(pid[currentProgram % programCount], SIGSTOP);
		}
		currentProgram++;
		if(waitpid(pid[currentProgram % programCount], &wstatus, WNOHANG) == 0){
			printf("Starting process %d\n", pid[currentProgram % programCount]);
			kill(pid[currentProgram % programCount], SIGCONT);
			alarm(3);
			sigwait(&set, &sig);
			cycle = 0;
		}
		else{
			cycle++;
			if(cycle == 2 * programCount){
				printf("All processes are finished.\n");
				break;
			}
		}
		

	}

			
		


	free(input);
	free(token);
}

void signal_init(){
	sigact.sa_handler = signal_handler;
	sigaction(SIGUSR1, &sigact, NULL);
	sigaction(SIGALRM, &sigact, NULL);
}

void signal_handler(int signal){
	
	printf("Child Process: %d - Received signal: %d\n", getpid(), signal);
	if(signal == SIGALRM){
		//printf("Process %d recieved signal %d\n", getpid(), signal);
		int i;
		printf("Stopping process %d\n", pid[currentProgram % programCount]);
		kill(pid[currentProgram % programCount], SIGSTOP);
		currentProgram++;
		printf("Stopping process %d\n", pid[currentProgram % programCount]);
		kill(pid[currentProgram % programCount], SIGCONT);
		alarm(3);


	}

}


void send_signal(pid_t *pool, int size, int sig){
	int j = 0;
	for(j = 0; j < size; j++){
		printf("Sending signal %d to child %d\n", sig, pool[j]);
		kill(pool[j], sig);
		
	}
}

int some_running(pid_t *pool, int size){
	int j, wstatus;
	for( j = 0; j < size; j++){
		if(waitpid(pool[j], &wstatus, WNOHANG) == 0){
			return 1;
		}
	}
	return 0;
}
