#include "RWLock.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
using std::cout;

RWLock myLock;

int contadorCompartido;

void* read(void *data);
void* write(void *data);
bool raceCondition();
bool starvation();

int main(int argc, char* argv[]) {
	cout << "Start Race Condition Test:\n";
	if(raceCondition()){
		cout << "Race Condition Test Success\n";
	}else{
		cout << "Race Condition Test Failed\n";	
	}

	cout << "Start Starvation Test:\n";
	if(starvation()){
		cout << "Starvation Test Success\n";
	}else{
		cout << "Starvation Test Failed\n";
	}
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
	printf("Thread %02d finish read. Contador:%d\n", id,contadorCompartido);
	myLock.runlock();
	pthread_exit(0);
}

void* write(void *data) 
{
	int id = *((int*) data);
	sleep(1);
	printf("Thread %02d req write\n", id);
	myLock.wlock();
	printf("Thread %02d start write. Contador:%d\n", id,contadorCompartido);
	sleep(1);
	contadorCompartido++;
	printf("Thread %02d finish write. Contador:%d\n", id,contadorCompartido);
	myLock.wunlock();
	pthread_exit(0);
}

bool raceCondition(){
	contadorCompartido = 0;
	pthread_t thread[100];
	int threadIds[100];

	//threads alternados entre read y write
	for(int i=0; i<100; i++){
		threadIds[i] = i;
		if(i % 2 == 0){
			pthread_create(&thread[i],NULL,write,&threadIds[i]);
		}else{
			pthread_create(&thread[i],NULL,read,&threadIds[i]);
		}
	}
	//espero finalización
	for(int i=0; i<100; i++){
		pthread_join(thread[i],NULL);
	}

	/*
	Si'hay una race condition entre 2 writes, el contador tendría que ser menor a 50,
	ya que si corren simultaneamente la lectura, el incremento del valor y el write-back,
	puede que los writes incrementen el valor al mismo tiempo, sin esperar a que el otro termine
	y finalmente se pierda al menos un incremento de valor
	*/
	return contadorCompartido == 50;
}

bool starvation(){
	contadorCompartido = 0;
	pthread_t threads[100];
	int threadIds[100];

	for (int i=0; i < 100; i++) 
	{
		threadIds[i] = i;
		if (i % 20 == 0){
			pthread_create(&threads[i], NULL, write, &threadIds[i]);
		}else{
			pthread_create(&threads[i], NULL, read, &threadIds[i]);
		}
	}

	for (int i=0; i < 100; i++){
		pthread_join(threads[i], NULL);
	}

	/*
	En este test, además de ver que no hay starvation, vemos claramente como hay múltiples lecturas
	concurrentes entre los pocos writes. Se puede ver que solo son frenadas por la aparición de writes, 
	que necesitan exclusividad en el manejo de la variable
	*/
	return contadorCompartido == 5;
}