#include "RWLock.h"

RWLock :: RWLock() {
    reading = 0;
    writing = 0;
    writers = 0;
}

void RWLock :: rlock() {
    while(1) {
        pthread_cond_wait(&turn, &m);
        if (writing <= 0 && writers <=0) {
            reading++;
            pthread_cond_signal(&turn);
            break;
        }
        pthread_cond_signal(&turn);
        pthread_cond_wait(&read_cond, &m_r);
    }
}

void RWLock :: wlock() {
    while(1) {
        pthread_cond_wait(&turn, &m);
        writers++;
        if (reading <= 0 && writing <=0) {
            writing++;
            writers--;
            pthread_cond_signal(&turn);
            break;
        }
        pthread_cond_signal(&turn);
        pthread_cond_wait(&write_cond, &m_w);
    }
}

void RWLock :: runlock() {
    pthread_cond_wait(&turn, &m);
    reading--;
    pthread_cond_signal(&turn);
    pthread_cond_signal(&write_cond);
}

void RWLock :: wunlock() {
    pthread_cond_wait(&turn, &m);
    writing--;
    if (writers > 0) {
        // Libero otro writer
        pthread_cond_signal(&write_cond);
    } else {
        // Libero reading
        pthread_cond_signal(&read_cond);
    }
    pthread_cond_signal(&turn);
}
