발전 계획 :
    멀티쓰레딩
    {
        현재 worker, watcher 구조체 설정, worker 함수 구현까지 완료.
        ** 막힌 지점: watcher에서 cond_signal을 받았을때 종료된 스레드의 client_fd를 어떻게 알아내어 pthread_join을 하고 client_fd 배열을 관리할지.
        -> 현재 구조가,
        client_fd배열 {0, 1, 2, 3, 4 ..}
        worker_id배열 {0, 1, 2, 3, 4 ..}
        에서 만약 2번 worker_id를 가진 스레드가 종료되었을 시 
        client_fd배열 {0, 1, 3, 4 ..}
        worker_id배열 {0, 1, 3, 4 ..} 로 땡겨야 함.
    }
    에코에서 단체채팅방으로. 
    클라이언트 이름 짓기. 
