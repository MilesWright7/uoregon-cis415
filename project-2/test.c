#include <unistd.h>
#include <stdio.h> 
#include <sys/wait.h>
#include <signal.h>

int main(){
	pid_t pid;
	sigset_t set;
	int sig;



	pid = fork();
	if(pid == 0){
		printf("child %d started\n", getpid());
		int i = 0;
		for(i = 0; i < 1000000000; i++){
			// nothing
		}
	}
	else{
		sigemptyset(&set);
		sigaddset(&set, SIGALRM);
		printf("Parent %d\n", getpid());
		alarm(5);
		sigwait(&set, &sig);
		printf("Parent %d\n", getpid());
	}

}
