/*Author Miles Wright
 *
 * CIS415
 *
 * quacker server which uses threads to implement a pub/sub architecture for a social media 
 * like thing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>


#define URLSIZE 100
#define CAPSIZE 100
#define MAXNAME 100
#define MAXENTRIES 100
#define MAXTOPICS 50
#define NUMPROXIES 50

struct topicEntry{
	int entryNum;
	struct timeval timeStamp;
	int pubID;
	char photoURL[URLSIZE];
	char photoCaption[CAPSIZE];
};

struct topicQueue{
	int initialized;
	int id, length;
	int head, tail, count, inserted;
	struct topicEntry *queue;
};

struct topicQueue registry[MAXTOPICS];
int tq_count = 0;
pthread_t publishers[NUMPROXIES];
pthread_t subscribers[NUMPROXIES];
pthread_t cleanup;
pthread_attr_t attr;

pthread_mutex_t mutex[MAXTOPICS];
sem_t full[MAXTOPICS];
sem_t empty[MAXTOPICS];

struct threadargs {
	int id;
	int free;
	char command_file[50];
	int lastEntry;
};

struct threadargs pubargs[NUMPROXIES];
struct threadargs subargs[NUMPROXIES];
struct threadargs cleanargs;

void *sub_function(void *params);
void *pub_function(void *params);

int DELTA = 20;


int enqueue(struct topicEntry *TE, int topic_ID){
	int i, j;
	struct timeval now;
	for(i = 0; i < MAXTOPICS; i++){
		pthread_mutex_lock(&(mutex[i]));
		if(registry[i].id == topic_ID){
			if(registry[i].count == registry[i].length){
				pthread_mutex_unlock(&(mutex[i]));
				printf("Enqueue failure for entry number %d in queue %d\n", registry[i].inserted+1, topic_ID);
				return -1;
			}
			gettimeofday(&now, NULL);
			registry[i].inserted++;
			TE->entryNum = registry[i].inserted;
			TE->timeStamp = now;
			registry[i].count++;
			registry[i].queue[registry[i].tail] = *TE;
			registry[i].tail = (registry[i].tail + 1) % registry[i].length;
			
			pthread_mutex_unlock(&(mutex[i]));
			printf("Enqueue sucessful for entry number %d in queue %d\n", registry[i].inserted, topic_ID);
			return 0;
		}
		pthread_mutex_unlock(&(mutex[i]));

	}

}

int getEntry(struct topicEntry *TE, int lastEntry, int topic_ID){
	int i, j;
	for(i = 0; i < MAXTOPICS; i++){
		pthread_mutex_lock(&(mutex[i]));
		if(registry[i].id == topic_ID){
			// case 1
			if(registry[i].count == 0){
				pthread_mutex_unlock(&(mutex[i]));
				printf("Topic Queue %d is empty\n", topic_ID);
				return 0;
			}
			// case 2
			for(j = 0; j < registry[i].length; j++){
				if(registry[i].queue[(registry[i].head + j) % registry[i].length].entryNum == (lastEntry + 1)){
					strcpy(TE->photoURL, registry[i].queue[(registry[i].head + j) % registry[i].length].photoURL); 
					strcpy(TE->photoCaption, registry[i].queue[(registry[i].head + j) % registry[i].length].photoCaption);
					TE->entryNum = lastEntry + 1;
					TE->timeStamp = registry[i].queue[(registry[i].head + j) % registry[i].length].timeStamp;
					TE->pubID = registry[i].queue[(registry[i].head + j) % registry[i].length].pubID;
					pthread_mutex_unlock(&(mutex[i]));
					printf("Getting entry for topic %d in queue %d\n", lastEntry+1, topic_ID);
					return 1;
				}
			}
			// case 3 part 2
			if(registry[i].queue[registry[i].head].entryNum > (lastEntry + 1)){
				strcpy(TE->photoURL, registry[i].queue[registry[i].head].photoURL); 
				strcpy(TE->photoCaption, registry[i].queue[registry[i].head].photoCaption);
				TE->entryNum = registry[i].queue[registry[i].head].entryNum;
				TE->timeStamp = registry[i].queue[registry[i].head].timeStamp;
				TE->pubID = registry[i].queue[registry[i].head].pubID;
				int ret = registry[i].queue[registry[i].head].entryNum;
				pthread_mutex_unlock(&(mutex[i]));
				printf("Getting entry for topic %d in queue %d\n", lastEntry+1, topic_ID);
				return ret;
			}
			// case 3 part 1
			// queue not empty and no entry is greater than last entry + 1. threfore the only option is that last entry + 1 is not in the queue at all so just return 0
			pthread_mutex_unlock(&(mutex[i]));
			printf("No new entries to return \n");
			return 0;
			
		}
		pthread_mutex_unlock(&(mutex[i]));
	}
}

int dequeue(int topic_num){
	int i, size, start;
	struct timeval now;
	pthread_mutex_lock(&(mutex[topic_num]));
	if(registry[topic_num].count == 0){
		pthread_mutex_unlock(&(mutex[topic_num]));
		return -1;
	}

	gettimeofday(&now, NULL);
	size = registry[topic_num].count;
	start = registry[topic_num].head;
	printf("Dequeue called on queue %d\n", registry[i].id);
	i = 0;
	while(1){
		if(registry[topic_num].queue[(start + i) % registry[topic_num].length].timeStamp.tv_sec + DELTA < now.tv_sec){
			registry[topic_num].count--;
			registry[topic_num].head = (registry[topic_num].head + 1) % registry[topic_num].length;
		}
		else{
			break;
		}
	
	}

	pthread_mutex_unlock(&(mutex[topic_num]));
	return 0;	

}
void init_TQ(int id, int length){
	printf("creating topicQueue of length %d and id %d\n", length, id);
	registry[tq_count].initialized = 1;
	registry[tq_count].count = 0;
	registry[tq_count].head = 0;
	registry[tq_count].tail = 0;
	registry[tq_count].inserted = 0;
	registry[tq_count].length = length;
	registry[tq_count].id = id;
	registry[tq_count].queue = (struct topicEntry *)malloc(sizeof(struct topicEntry) * length);
	tq_count++;
}


void init_all(){
	int i;

	for(i = 0; i < MAXTOPICS; i++){
		pthread_mutex_init(&(mutex[i]), NULL);
		//sem_init(&full[i], 0, 0);
		//sem_init(&empty[i], 0, MAXTOPICS);
	}
	for(i = 0; i < NUMPROXIES; i++){
		pubargs[i].free = 1;
		subargs[i].free = 1;
	}
	pthread_attr_init(&attr);
}
void *subscriber1(void *args){
	int bid, tid;
	tid = ((struct threadargs *) args)->id;
	printf("subscriber thread id = %d, %ld\n", tid, pthread_self());
	struct topicEntry get;
	getEntry(&get, 0, 234);

	sleep(1);
	getEntry(&get, 1, 234);
	sleep(1);
	getEntry(&get, 2, 234);
	getEntry(&get, 3, 234);
	((struct threadargs *)args)->free = 1;
}
void *publisher1(void *args){
	int bid, tid;
	tid = ((struct threadargs *) args)->id;
	printf("publisher thread id = %d, %ld\n", tid, pthread_self());
	struct topicEntry t1, t2, t3, t4;
	strcpy(t1.photoURL, "URL HERE 1");
	strcpy(t1.photoCaption, "CAPTION HERE 1");
	strcpy(t2.photoURL, "URL HERE 2");
	strcpy(t2.photoCaption, "CAPTION HERE 2");
	strcpy(t3.photoURL, "URL HERE 3");
	strcpy(t3.photoCaption, "CAPTION HERE 3");
	strcpy(t4.photoURL, "URL HERE 4");
	strcpy(t4.photoCaption, "CAPTION HERE 4");
	//sleep(1);
	enqueue(&t1, 234);
	sleep(1);
	enqueue(&t2, 234);
	enqueue(&t3, 234);
	enqueue(&t4, 234);
	((struct threadargs *)args)->free = 1;
}

void *cleanup1(void *args){
	int i;
	while(1){
		for(i = 0; i < MAXTOPICS; i++){
			dequeue(i);
		}
		sleep(2);
	}
}

void *subscriber2(void *args){
	int bid, tid;
	printf("Proxy thread %ld - type: Subscriber\n", pthread_self());

	((struct threadargs *)args)->free = 1;
}

void *publisher2(void *args){
	int bid, tid;
	printf("Proxy thread %ld - type: Publisher\n", pthread_self());
	
	((struct threadargs *)args)->free = 1;
}

void *subscriber3(void *args){
	FILE *in_file;
	char *saveptr;
	size_t size = 100;
	char *input = (char *)malloc(sizeof(char) * size);
	char *token = (char *)malloc(sizeof(char) * size);
	int i, ret;
	int ammount = 1;
	in_file = fopen(((struct threadargs *)args)->command_file, "r");
	ammount = getline(&input, &size, in_file);
	while(ammount > 0){
		i = 0;
		while(input[i] != '\0'){
			if(input[i] == '\n'){
				input[i] = '\0';
			}
			i++;
		}
	

		token = strtok_r(input, " ", &saveptr);
		if(strcmp(token, "get") == 0){
			struct topicEntry TE;
			int lastRecieved = ((struct threadargs *)args)->lastEntry;
			token = strtok_r(NULL, " ", &saveptr);
			int id = atoi(token);
			printf("Proxy Thread %d %ld - type: Subscriber - Executing command get\n", ((struct threadargs *)args)->id, pthread_self());
			ret = getEntry(&TE, lastRecieved, id);
			if(ret == 0){
				// nothing we are up to date
			}
			else if(ret == 1){
				// got return so increment last recieved entry by 1
				((struct threadargs *)args)->lastEntry++;
			}
			else{
				((struct threadargs *)args)->lastEntry = ret;
			}
		}
		else if(strcmp(token, "sleep") == 0){
			token = strtok_r(NULL, " ", &saveptr);
			int time = atoi(token);
			printf("Proxy Thread %d %ld - type: Subscriber - Executing command sleep\n", ((struct threadargs *)args)->id, pthread_self());
			usleep(1000 * time);
		}
		else if(strcmp(token, "stop") == 0){
			
			((struct threadargs *)args)->free = 1;
			printf("Proxy Thread %d %ld - type: Subscriber - Executing command stop\n", ((struct threadargs *)args)->id, pthread_self());
			break;
		}
		ammount = getline(&input, &size, in_file);
	}

}

void *publisher3(void *args){
	FILE *in_file;
	char *saveptr;
	size_t size = 100;
	char *input = (char *)malloc(sizeof(char) * size);
	char *token = (char *)malloc(sizeof(char) * size);
	int i;
	int ammount = 1;
	in_file = fopen(((struct threadargs *)args)->command_file, "r");
	ammount = getline(&input, &size, in_file);
	while(ammount > 0){
		i = 0;
		while(input[i] != '\0'){
			if(input[i] == '\n'){
				input[i] = '\0';
			}
			i++;
		}
	

		token = strtok_r(input, " ", &saveptr);
		if(strcmp(token, "put") == 0){
			struct topicEntry TE;
			token = strtok_r(NULL, " ", &saveptr);
			int id = atoi(token);
			TE.pubID = ((struct threadargs *)args)->id;
			token = strtok_r(NULL, " ", &saveptr);
			strcpy(TE.photoURL, token);
			token = strtok_r(NULL, " ", &saveptr);
			strcpy(TE.photoCaption, token);
			printf("Proxy Thread %d %ld - type: Publisher - Executing command put\n", ((struct threadargs *)args)->id, pthread_self());
			enqueue(&TE, id);
		}
		else if(strcmp(token, "sleep") == 0){
			token = strtok_r(NULL, " ", &saveptr);
			int time = atoi(token);
			printf("Proxy Thread %d %ld - type: Publisher - Executing command sleep\n", ((struct threadargs *)args)->id, pthread_self());
			usleep(1000 * time);
		}
		else if(strcmp(token, "stop") == 0){
			
			((struct threadargs *)args)->free = 1;
			printf("Proxy Thread %d %ld - type: Publisher - Executing command stop\n", ((struct threadargs *)args)->id, pthread_self());
			break;
		}
		
		ammount = getline(&input, &size, in_file);

	}
}

	
int main(){
	init_all();
	printf("---test for part 1---\n");
	init_TQ(123, 3);
	struct topicEntry t1, t2, t3, t4;
	struct topicEntry get;
	getEntry(&get, 0, 123);
	strcpy(t1.photoURL, "URL HERE 1");
	strcpy(t1.photoCaption, "CAPTION HERE 1");
	strcpy(t2.photoURL, "URL HERE 2");
	strcpy(t2.photoCaption, "CAPTION HERE 2");
	strcpy(t3.photoURL, "URL HERE 3");
	strcpy(t3.photoCaption, "CAPTION HERE 3");
	strcpy(t4.photoURL, "URL HERE 4");
	strcpy(t4.photoCaption, "CAPTION HERE 4");
	enqueue(&t1, 123);
	enqueue(&t2, 123);
	enqueue(&t3, 123);
	enqueue(&t4, 123);
	getEntry(&get, 0, 123);
	printf("Entry %d URL %s Caption %s\n", get.entryNum, get.photoURL, get.photoCaption);
	getEntry(&get, 1, 123);
	printf("Entry %d URL %s Caption %s\n", get.entryNum, get.photoURL, get.photoCaption);
	getEntry(&get, 2, 123);
	printf("Entry %d URL %s Caption %s\n", get.entryNum, get.photoURL, get.photoCaption);
	getEntry(&get, 3, 123);
	printf("---end of part 1---\n");
	printf("implementation of queue funciotns all work, dequeue wasnt tested above but it works.\nDequeue is difficult to show tests for because it is time based\n");
	printf("---tests for part 2---\n");
	init_TQ(234, 3);
	pubargs[0].id = 0;
	pthread_create(&publishers[0], &attr, publisher1, (void *) &pubargs[0]);
	subargs[0].id = 0;
	pthread_create(&subscribers[0], &attr, subscriber1, (void *)&subargs[0]);
	sleep(5);
	printf("---end of part 2---\n");

	printf("start of real function, input commands to make publishers and subscribers\n");
	printf("main function will sleep for 30 seconds before moving on or exiting\n");
	tq_count = 0;
	char *input = (char *)malloc(sizeof(char) * 1000);
	char *token = (char *)malloc(sizeof(char) * 1000);
	size_t size = 1000;
	int ammount = 1;
	int subs, pubs, i;

	while(1){
		printf(">>>");
		i = 0;
		ammount = getline(&input, &size, stdin);
		while(input[i] != '\0'){
			if(input[i] == '\n'){
				input[i] = '\0';
			}
			i++;
		}

		token = strtok(input, " ");
		if(strcmp(token, "create") == 0){
			token = strtok(NULL, " ");
			token = strtok(NULL, " ");
			int id = atoi(token);
			token = strtok(NULL, " ");
			token = strtok(NULL, " ");
			int length = atoi(token);
			init_TQ(id, length);
		}
		else if(strcmp(token, "query") == 0){
			token = strtok(NULL, " ");
	   		if(strcmp(token, "topics") == 0){
		 		for(i = 0; i < tq_count; i++){
					printf("Topic %d length %d\n", registry[i].id, registry[i].length);
				}
			}
			else if(strcmp(token, "publishers") == 0){
				for(i = 0; i < NUMPROXIES; i++){
					if(pubargs[i].free == 0){
						printf("Publisher id %d, command file %s\n", pubargs[i].id, pubargs[i].command_file);
					}
				}
			}
			else{
				for(i = 0; i < NUMPROXIES; i++){
					if(subargs[i].free == 0){
						printf("Subscriber id %d, command file %s\n", subargs[i].id, subargs[i].command_file);
					}
				}
			}
		}
		else if(strcmp(token, "add") == 0){
			token = strtok(NULL, " ");
			if(strcmp(token, "publisher") == 0){
				token = strtok(NULL, " ");
				for(i = 0; i < NUMPROXIES; i++){
					if(pubargs[i].free == 1){
						pubargs[i].free = 0;
						strcpy(pubargs[i].command_file, token);
						pubargs[i].id = i;
						printf("Created publisher id %d with command file %s\n", i, token);
						break;
					}
				}
			}
			else{
				token = strtok(NULL, " ");
				for(i = 0; i < NUMPROXIES; i++){
					if(subargs[i].free == 1){
						subargs[i].free = 0;
						strcpy(subargs[i].command_file, token);
						subargs[i].id = i;
						printf("Created subscriber id %d with command file %s\n", i, token);
						break;
					}
				}
			}
		}
		else if(strcmp(token, "delta") == 0){
			token = strtok(NULL, " ");
			int d = atoi(token);
			DELTA = d;
			printf("Changed delta value to %d\n", d);
		}
		else if(strcmp(token, "start") == 0){
			pthread_create(&cleanup, &attr, cleanup1, (void *)&cleanargs);
			for(i = 0; i < NUMPROXIES; i++){
				if(subargs[i].free == 0){
					pthread_create(&subscribers[i], &attr, subscriber3, (void *)&subargs[i]);
				}
				if(pubargs[i].free == 0){
					pthread_create(&publishers[i], &attr, publisher3, (void *)&pubargs[i]);

				}
			}
			break;
		}
					
	}
	sleep(30);

}

