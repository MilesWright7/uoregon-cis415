/* author Miles Wright
 * Lab 6 signal sending
 */

#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

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
		time_t start, end;
		double running = 0;
		start = time(NULL);
		while(running < 10.0){
			end = time(NULL);
			running = difftime(end, start);

			printf("    Child Process %d - Still alive after for %.1f seconds.\n", getpid(), running);
			usleep(100000);
		}
		exit(EXIT_SUCCESS);
	}
	else if(pid > 0){
		printf("Parent %d sending signals to children.\n", getpid());
		while(waitpid(pid, &wstatus, WNOHANG) == 0){
			sleep(1);
			kill(pid, SIGSTOP);
			sleep(3);
			kill(pid, SIGCONT);
		}
	}
}
