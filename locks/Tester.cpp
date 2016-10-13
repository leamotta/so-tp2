#include "RWLock.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
using std::cout;

RWLock myLock;

void* read(void *data);
void* write(void *data);
void deadlock();
void starvation();

int main(int argc, char* argv[]) {
	cout << "Start Deadlock Test:\n";
	deadlock();
	cout << "Deadlock Test Finished\n";
	cout << "Start Starvation Test:\n";
	starvation();
	cout << "Starvation Test Finished\n";
	return 0;
}

void* read(void *data) 
{
	int id = *((int*) data);
	sleep(1);
	printf("Thread %02d req read\n", id);
	myLock.rlock();
	printf("Thread %02d start read\n", id);
	sleep(1);
	printf("Thread %02d finish read\n", id);
	myLock.runlock();
	pthread_exit(0);
}

void* write(void *data) 
{
	int id = *((int*) data);
	sleep(1);
	printf("Thread %02d req write\n", id);
	myLock.wlock();
	printf("Thread %02d start write\n", id);
	sleep(1);
	printf("Thread %02d finish write\n", id);
	myLock.wunlock();
	pthread_exit(0);
}

void deadlock(){
	pthread_t thread[100];
	int threadIds[100];
	for (int i = 0; i < 100; i++) {
		threadIds[i] = i;
	}
	//threads alternados entre read y write
	for(int i=0; i<100; i++){
		if(i % 3 == 0){
			pthread_create(&thread[i],NULL,write,&threadIds[i]);
		}else{
			pthread_create(&thread[i],NULL,read,&threadIds[i]);
		}
	}
	//espero finalización
	for(int i=0; i<100; i++){
		pthread_join(thread[i],NULL);
	}
}

void starvation(){
	pthread_t threads[100];
	int threadIds[100];

	for (int i=0; i < 100; i++) 
	{
		threadIds[i] = i;
		if (i % 10 == 0){
			pthread_create(&threads[i], NULL, write, &threadIds[i]);
		}else{
			pthread_create(&threads[i], NULL, read, &threadIds[i]);
		}
	}

	for (int i=0; i < 100; i++){
		pthread_join(threads[i], NULL);
	}
}