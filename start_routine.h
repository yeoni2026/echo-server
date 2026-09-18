#pragma once

#include <pthread.h>

typedef struct {
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
    int *client_fd;
    size_t *nthread;
} share_arg;

typedef struct {
    share_arg* share; 
    pthread_t *worker_id;
} watcher_arg;

typedef struct {
    share_arg* share; 
    int my_client_fd;
} worker_arg;

void* watcher(void* arg);
void* worker(void* arg);