#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <assert.h>
#include <sys/wait.h>
#include <pthread.h>
#include "start_routine.h"

#define PORT 8080
#define MAX_NTHREAD 50

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0; // Ctrl+C 입력 시 루프 탈출 신호
}

void deletePid(pid_t *pid_array, size_t pid_size, pid_t value){
    int change_position = 0;
    for (size_t i = 0; i < pid_size; ++i){
        if (change_position){
            pid_array[i - 1] = pid_array[i];
        } else {
            if (pid_array[i] == value){
                change_position = 1;
            }
        }
    }
}

int main(){
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    int server_fd;
    struct sockaddr_in server_addr;
    /*
    struct sockaddr_in {
    sa_family_t    sin_family; // 주소 체계 (IPv4이므로 항상 AF_INET)
    in_port_t      sin_port;   // 16비트 포트 번호 (빅 엔디안, htons 적용)
    struct in_addr sin_addr;   // 32비트 IPv4 주소 (inet_pton, htonl 적용)
    char           sin_zero[8];// 크기를 맞추기 위한 8바이트 패딩 (반드시 0으로 초기화)
    }; */
    
    // 소켓 생성 (IPv4, TCP 스트림)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1){
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    // 주소 구조체 초기화 및 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // 소켓과 IP/PORT 결합.
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        perror("바인드 실패");
        close(server_fd);
        exit(EXIT_FAILURE);
    }    

    // 클라이언트 접속 대기열 생성.
    if (listen(server_fd, 5) == -1){
        perror("리슨 실패");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("서버 준비 완료: 포트 %d에서 연결 대기 중...\n", PORT);

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    pid_t pid_array[64];
    pid_t worker_pid;
    size_t pid_size = 0;

    pthread_t *watcher_id = malloc(sizeof(pthread_t));
    pthread_t *worker_id = malloc(sizeof(pthread_t) * MAX_NTHREAD);
    size_t nthread = 0;

    // watcher 구조체 설정.
    watcher_arg *wc_arg = malloc(sizeof(watch_arg));
    wc_arg->worker_id = worker_id;
    wc_arg->nthread = &nthread;
    
    pthread_create(watcher_id, NULL, watcher, wc_arg);

    // 새 통신용 소켓(client_fd) 발급 (연결될 때까지 프로세스는 Blocked)
    while(keep_running){
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        
        if (client_fd == -1){
            perror("접속 수락 실패");
            continue;
        }
        
        printf("클라이언트 연결 성공!\n");

        ++nthread;
        pthread_create(&thread_array[nthread], NULL, worker, (void *)&client_fd);
        // arg에 담을것 : client_fd, 하나? 
    }

    for (size_t i = 0; i < pid_size; ++i){
        kill(pid_array[i], SIGTERM);
    }
    while(wait(NULL) > 0);

    close(server_fd);
    return 0;
}