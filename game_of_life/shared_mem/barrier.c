#include "barrier.h"

const int THREAD_IS_THE_LAST_ONE = 1;

int thread_barrier_init(thread_barrier *barrier, unsigned limit) {
    int error_code = 0;
    if (limit == 0) {
        return -1;
    }

    error_code = pthread_mutex_init(&barrier->mutex, NULL);
    if (error_code != 0) {
        return error_code;
    }

    error_code = pthread_cond_init(&barrier->condition, NULL);
    if (error_code != 0) {
        pthread_mutex_destroy(&barrier->mutex);
        return error_code;
    }

    barrier->limit = limit;
    barrier->count = 0;
    barrier->phase = 0;
    return 0;
}

int thread_barrier_destroy(thread_barrier *barrier) {
    if (pthread_mutex_destroy(&barrier->mutex) != 0
        || pthread_cond_destroy(&barrier->condition) != 0) {
        return -1;
    }
    return 0;
}

int thread_barrier_wait(thread_barrier *barrier) {
    int error_code = pthread_mutex_lock(&barrier->mutex);
    if (error_code != 0) {
        return error_code;
    }

    barrier->count++;
    if (barrier->count == barrier->limit) {
        barrier->count = 0;
        barrier->phase = 1 - barrier->phase;
        pthread_cond_broadcast(&barrier->condition);
        pthread_mutex_unlock(&barrier->mutex);
        return THREAD_IS_THE_LAST_ONE;
    } else {
        int my_phase = 1 - barrier->phase;
        while (my_phase != barrier->phase) {
            pthread_cond_wait(&barrier->condition, &barrier->mutex);
        }
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}