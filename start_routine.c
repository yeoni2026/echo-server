#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include "start_routine.h"

typedef struct {
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;
    int my_client_fd;
    size_t *nthread;
} worker_clean_arg;

static void worker_cleanup(void *arg) {
    worker_clean_arg *wc_arg = (worker_clean_arg *)arg;
    
    pthread_mutex_lock(wc_arg->mutex);

    close(wc_arg->my_client_fd);
    --(*wc_arg->nthread);
    pthread_cond_signal(wc_arg->cond);

    pthread_mutex_unlock(wc_arg->mutex);
}

void* worker(void* arg){
    pthread_detach(pthread_self());
    worker_arg *wk_arg = (worker_arg *)arg;

    pthread_mutex_lock(wk_arg->mutex);
    pthread_t me = pthread_self();
    int found_idx = -1;

    for (size_t i = 0; i < *wk_arg->nthread; ++i) {
        if (pthread_equal(wk_arg->worker_id[i], me)) {
            found_idx = i;
            break;
        }
    }
    pthread_mutex_unlock(wk_arg->mutex);

    assert(found_idx >= 0);
    int my_client_fd = wk_arg->client_fd[found_idx];

    worker_clean_arg wc_arg;
    wc_arg.cond = wk_arg->cond;
    wc_arg.mutex = wk_arg->mutex;
    wc_arg.my_client_fd = my_client_fd;
    wc_arg.nthread = wk_arg->nthread;
    pthread_cleanup_push(worker_cleanup, &wc_arg);

    // 데이터 수신 (커널 수신 큐 -> 유저 buffer 복사)
    char buffer[1024];

    while (1){
        ssize_t bytes_read = read(my_client_fd, buffer, sizeof(buffer) - 1);
        
        if (bytes_read > 0){
            buffer[bytes_read] = '\0';
            printf("[%d번 FD] 수신된 메시지: %s\n", my_client_fd, buffer);

            //수신된 데이터를 그대로 다시 송신 (Echo)
            if (write(my_client_fd, buffer, bytes_read) == -1) {
                perror("write 실패");
                break; // 송신 실패 시 통신 루프 탈출
            }
        } else if (bytes_read == 0){
            printf("클라이언트가 연결을 종료했습니다.\n");
            break;
        } else {
            perror("읽기 실패");
            break;
        }
    }
    pthread_cleanup_pop(0);
    close(my_client_fd);

    pthread_mutex_lock(wk_arg->mutex);

    int last_idx = *wk_arg->nthread - 1;
    if (found_idx != last_idx) {
        wk_arg->client_fd[found_idx] = wk_arg->client_fd[last_idx];
        wk_arg->worker_id[found_idx] = wk_arg->worker_id[last_idx];
    }
    --(*wk_arg->nthread);
    pthread_cond_signal(wk_arg->cond);

    pthread_mutex_unlock(wk_arg->mutex);

    return NULL;
}