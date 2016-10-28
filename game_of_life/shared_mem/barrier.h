#ifndef barrier_h
#define barrier_h

#include <pthread.h>

extern const int THREAD_IS_THE_LAST_ONE;

typedef struct thread_barrier {
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    unsigned limit;
    unsigned count;
    unsigned phase;
} thread_barrier;

int thread_barrier_init(thread_barrier *barrier, unsigned limit);
int thread_barrier_destroy(thread_barrier *barrier);
int thread_barrier_wait(thread_barrier *barrier);

#endif /* barrier_h */
