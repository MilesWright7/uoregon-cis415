/*
* Description: A simple lab showing signal processing
*
* Author: Jared Hall
*
* Date: 10/21/2019
*
* Notes:
* 1. to be done in lab
*/

/*-------------------------Preprocessor Directives---------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
/*---------------------------------------------------------------------------*/
void signal_handler(int signal);
void signal_init();
struct sigaction sigact;
void send_signal(pid_t *pool, int size, int sig);

/*-----------------------------Program Main----------------------------------*/
int	 main()
{
	//variable definitions
	pid_t  pid[5];
	pid_t w;
	int wstatus, i;
	i=0;
	for(i = 0; i < 5; i++){
		pid[i] = fork();
	
		if (pid[i] == 0) {
			printf("	Child Process: %i - Starting infinite loop...\n", getpid());
			signal_init();
			while(1) {
				i++;
				if(i%10000000) {
					printf("	Child Process: %i - Running infinite loop...\n", getpid());
					i=0;
				}
			}
		} else {
			//else this is an existing proc i.e. the parent
		}
	}	
	printf("Parent Process: %i, Waiting for child to finish...\n", getpid());
	send_signal(pid, 5, SIGUSR1);
	send_signal(pid, 5, SIGUSR1);
	send_signal(pid, 5, SIGINT);
	for(i = 0; i < 5; i++){
		w = waitpid(pid[i], &wstatus, WUNTRACED | WCONTINUED);
	}
	printf("All child processes joined. Exiting.\n");
	

}
void signal_init(){
	sigact.sa_handler = signal_handler;
	sigaction(SIGUSR1, &sigact, NULL);
}
void signal_handler(int signal){
	sigset_t *set;
	printf("Child Process: %d - Received signal: %d\n", getpid(), signal);
	sigwait(set, &signal);
}
void send_signal(pid_t *pool, int size, int sig){
	int j = 0;
	for(j = 0; j < size; j++){
		kill(pool[j], sig);
	}
}

/*-----------------------------Program End-----------------------------------*/
