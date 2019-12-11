#include <sys/time.h>
#include <stdio.h>

int main(){
	struct timeval start, end;
	gettimeofday(&start, NULL);
	sleep(2);
	gettimeofday(&end, NULL);
	double time = end.tv_sec - start.tv_sec;
	printf("end time = %f\n", time);
}
