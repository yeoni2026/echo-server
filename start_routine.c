#include <unistd.h>
#include "start_routine.h"


void* watcher(void* arg){
    watcher_arg *wc_arg =  (watcher_arg *)arg;
    share_arg *share = wc_arg->share;

    pthread_mutex_lock(share->mutex);
    
}
void* worker(void* arg){
    worker_arg *wk_arg = (worker_arg *)arg;
    share_arg *share = wk_arg->share;

    // 데이터 수신 (커널 수신 큐 -> 유저 buffer 복사)
    char buffer[1024];

    while (1){
        ssize_t bytes_read = read(wk_arg->my_client_fd, buffer, sizeof(buffer) - 1);
        
        if (bytes_read > 0){
            buffer[bytes_read] = '\0';
            printf("[%ld번 FD] 수신된 메시지: %s\n", pid_idx, buffer);

            //수신된 데이터를 그대로 다시 송신 (Echo)
            if (write(wk_arg->my_client_fd, buffer, bytes_read) == -1) {
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
    close(wk_arg->my_client_fd);
    pthread_cond_signal(share->cond);
    return NULL;
}