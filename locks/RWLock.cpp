#include "RWLock.h"

RWLock :: RWLock() {
    reading = 0;
    writing = 0;
    writers = 0;
    pthread_mutex_init(&m, NULL);
	pthread_cond_init(&turn, NULL);
}

void RWLock :: rlock() {
    pthread_mutex_lock(&readMutex);
    pthread_mutex_lock(&m);
    while(writing){
		pthread_cond_wait(&turn, &m);
    }
    reading++;
    pthread_mutex_unlock(&m);
    pthread_mutex_unlock(&readMutex);

}

void RWLock :: wlock() {
	pthread_mutex_lock(&readMutex);
	pthread_mutex_lock(&m);
    writers++;
    while(writing || reading){
		pthread_cond_wait(&turn, &m);
    }
    writing++;
    pthread_mutex_unlock(&m);
}

void RWLock :: runlock() {
    pthread_mutex_lock(&m);
    reading--;
    if(reading == 0){
		pthread_cond_broadcast(&turn);
    }
	pthread_mutex_unlock(&m);
}

void RWLock :: wunlock() {
	pthread_mutex_unlock(&readMutex);
    pthread_mutex_lock(&m);
    writing--;
    writers--;
    pthread_cond_broadcast(&turn);
    pthread_mutex_unlock(&m);
}
