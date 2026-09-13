#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    if (argc != 3){
        printf("Usage: ./worker <client_fd> <pid_idx>\n");
        exit(EXIT_FAILURE);
    }
    long client_fd = strtol(argv[1], NULL, 10);
    long pid_idx = strtol(argv[2], NULL, 10);

    // 데이터 수신 (커널 수신 큐 -> 유저 buffer 복사)
    char buffer[1024];

    while (1){
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        
        if (bytes_read > 0){
            buffer[bytes_read] = '\0';
            printf("[%ld번 FD] 수신된 메시지: %s\n", pid_idx, buffer);

            //수신된 데이터를 그대로 다시 송신 (Echo)
            if (write(client_fd, buffer, bytes_read) == -1) {
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
    close(client_fd);
    exit(0);
}