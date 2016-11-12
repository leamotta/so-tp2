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
bool starvation(bool writeHeavy);

int main(int argc, char* argv[]) {
	cout << "Start Race Condition Test:\n";
	if(raceCondition()){
		cout << "Race Condition Test Success\n";
	}else{
		cout << "Race Condition Test Failed\n";	
	}

	cout << "Start Starvation Test:\n";
	if(starvation(false)){
		cout << "Starvation ReadHeavy Test Success\n";
	}else{
		cout << "Starvation ReadHeavy Test Failed\n";
	}
	if(starvation(true)){
		cout << "Starvation WriteHeavy Test Success\n";
	}else{
		cout << "Starvation WriteHeavy Test Failed\n";
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
	y finalmente se pierda al menos un incremento de valor.

	Sin embargo, este test no garantiza que no haya race conditions eventualemente, es solo un ejemplo
	de un caso particular en el que no sucede.
	*/
	return contadorCompartido == 50;
}

bool starvation(bool writeHeavy){
	contadorCompartido = 0;
	pthread_t threads[100];
	int threadIds[100];

	for (int i=0; i < 100; i++) {
		threadIds[i] = i;
		if (i % 20 == 0){
			if(writeHeavy){
				pthread_create(&threads[i], NULL, read, &threadIds[i]);
			}else{
				pthread_create(&threads[i], NULL, write, &threadIds[i]);	
			}
		}else{
			if(writeHeavy){
				pthread_create(&threads[i], NULL, write, &threadIds[i]);
			}else{
				pthread_create(&threads[i], NULL, read, &threadIds[i]);	
			}
		}
	}

	for (int i=0; i < 100; i++){
		pthread_join(threads[i], NULL);
	}

	/*
	En este test, vemos que no hay starvation de lecturas y escrituras, todas siguen un orden similar 
	a cuando fueron creadas y no hay ningún caso en el queden relegadas y tengan que esperar que terminen 
	todas las otras lecturas o todas las otras escrituras según el caso.

	Vemos claramente como hay múltiples lecturas concurrentes entre los pocos writes, en el caso que no es writeHeavy.
	Se puede ver que solo son frenadas por la aparición de writes, que necesitan exclusividad en el manejo de la variable.

	Además se ve que los writes están separados completamente de las lecturas, es decir, no se puede escribir
	mientras hay lecturas. Y en el caso writeHeavy vemos que no hay escrituras concurrentes, tardando un segundo
	cada write por separado

	*/
	return (writeHeavy)?contadorCompartido == 95:contadorCompartido==5;
}