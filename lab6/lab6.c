/* author Miles Wright
 * Lab 6 signal sending
 */

#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>


int main()
{
	pid_t pid, w;
	int wstatus, eStatus;
	int i = 0;

	pid = fork();
	if(pid < 0){
		perror("couldnt make new process\n");
		exit(EXIT_FAILURE);
	}
	else if(pid == 0){
		while(i < 10){

			printf("    Child Process %d - Still alive after for %d seconds.\n", getpid(), i);
			i++;
			sleep(1);
		}
		exit(EXIT_SUCCESS);
	}
	else if(pid > 0){
		printf("Parent %d sending signals to children.\n", getpid());
		while(pid > 0){
			sleep(1);
			kill(pid, SIGSTOP);
			sleep(3);
			kill(pid, SIGCONT);
		}
	}
}
