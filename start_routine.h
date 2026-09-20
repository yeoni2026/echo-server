#pragma once

#include <pthread.h>

typedef struct {
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
    int *client_fd;
    pthread_t *worker_id;
    size_t *nthread;
} worker_arg;

void* worker(void* arg);