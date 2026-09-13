# 실제 파일이 아닌 명령어 단축키들을 등록 (강제 실행 보장)
.PHONY: all clean run_server run_client

# 컴파일러와 기본 옵션 설정
CC = gcc
CFLAGS = -Wall -Wextra -O2

# 1. 'make'만 입력했을 때 실행될 기본 타겟 (둘 다 빌드)
all: server client worker

# 2. server.c와 worker.c를 묶어서 server 실행 파일 만들기
server: server.c
	$(CC) $(CFLAGS) -o server server.c

# 3. client.c를 client 실행 파일 만들기
client: client.c
	$(CC) $(CFLAGS) -o client client.c

worker: worker.c
	$(CC) $(CFLAGS) -o worker worker.c
# ----------------------------------------

# 'make run_server' 치면 빌드 후 바로 서버 실행
run_server: server
	./server

# 'make run_client' 치면 빌드 후 바로 클라이언트 실행
run_client: client
	./client

# 'make clean' 치면 생성된 실행 파일들 깔끔하게 삭제
clean:
	rm -f server client worker
