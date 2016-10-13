#ifndef RWLock_h
#define RWLock_h
#include <iostream>
#include <pthread.h>

class RWLock {
    public:
        RWLock();
        void rlock();
        void wlock();
        void runlock();
        void wunlock();

    private:
        pthread_mutex_t m;
        pthread_cond_t  turn;   /* Event: someone else's turn */

        pthread_mutex_t m_r;
        pthread_cond_t  read_cond;   

        pthread_mutex_t m_w;
        pthread_cond_t  write_cond;   

        int reading;
        int writing;
        int writers;
};

#endif
