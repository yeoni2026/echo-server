#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // 소켓 생성
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1){
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0){
        perror("잘못된 IP 주소");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
        perror("연결 실패");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    printf("서버(%s:%d) 연결 성공!\n", SERVER_IP, PORT);

    char message[100];

    printf("메시지를 입력하세요: ");
    while (fgets(message, sizeof(message), stdin) != NULL){
        message[strcspn(message, "\n")] = '\0';    
        if (write(sock_fd, message, strlen(message)) == -1) {
            perror("write 실패");
            break; // 송신 실패 시 통신 루프 탈출
        }
        
        ssize_t bytes_read = read(sock_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0){
            buffer[bytes_read] = '\0';
            printf("받은 메시지: %s\n", buffer);
        }
        printf("메시지를 입력하세요: ");
    }
    
    close(sock_fd);
    return 0;
}