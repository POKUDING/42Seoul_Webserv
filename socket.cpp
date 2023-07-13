#include <iostream>
#include <sys/socket.h>

// #include <stddef.h>
#include <sys/types.h>
#include <sys/event.h>

#include <netinet/in.h>
// #include <netinet/ip.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
#include <csignal>
#include <fcntl.h>

#include "errorHandle.hpp"
#include "basic.hpp"

#define MAX_EVENTS 10
#define BACKLOG 128

// using namespace std;


// void handler(int sig)
// {
// 	std::cout << sig << std::endl;
// 	close(client_fd);
//     close(server_fd);
//     close(kq);
// }

int main() {
    int client_fd;
    struct kevent event, events[MAX_EVENTS];
    int nev;

	// signal(SIGINT, &handler);

    // 서버 소켓 생성
	// 왜 SOCK_NONBLOCK 안되지
    int server_fd = socket(AF_INET, SOCK_STREAM,  0);
    if (server_fd == -1) {
		errorHandle::printError();
		exit(EXIT_FAILURE);
    }

	int optval = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		errorHandle::printError();
        close(server_fd);
		exit(EXIT_FAILURE);
	}

    // 서버 소켓 주소 설정 (bind시 사용)
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;			//IPv4 인터넷 프로토콜
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	//자동할당옵션
    server_addr.sin_port = htons(SERVER_PORT);	//사용할 포트 (hton short로 변경한 값 저장)

    // 서버 소켓에 바인딩
    if (bind(server_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
		errorHandle::printError();
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 연결 대기 상태로 진입
    if (listen(server_fd, BACKLOG) == -1) {
		errorHandle::printError();
        close(server_fd);
        exit(EXIT_FAILURE);
    }

	//소켓 논블록처리
	if (fcntl(server_fd, F_SETFL, O_NONBLOCK) == -1) {
		errorHandle::printError();
        	close(server_fd);
		exit(EXIT_FAILURE);
	}

    // kqueue 객체 생성
    int kq = kqueue();
    if (kq == -1) {
        errorHandle::printError();
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 서버 소켓을 kqueue에 등록
    EV_SET(&event, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
        errorHandle::printError();
        close(server_fd);
        close(kq);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << SERVER_PORT << std::endl;

    while (true) {
        // 이벤트 감지
        nev = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
        if (nev == -1) {
            perror("Kqueue event detection failed");
            close(server_fd);
            close(kq);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nev; i++) {
            if (events[i].ident == (uintptr_t)server_fd) {
                // 새로운 연결 요청 수락
				struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                client_fd = accept(server_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
                if (client_fd == -1) {
                    perror("Accept failed");
                    close(server_fd);
                    close(kq);
                    exit(EXIT_FAILURE);
                }

                std::cout << "New client connected: " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
                
                // 새로운 클라이언트 소켓을 kqueue에 등록
                EV_SET(&event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
                    perror("Kqueue event registration failed");
                    close(server_fd);
                    close(client_fd);
                    close(kq);
                    exit(EXIT_FAILURE);
                }
            } else {
                // 클라이언트로부터의 데이터 읽기
                char buffer[1024];
                // ssize_t bytes_read = read(events[i].ident, buffer, sizeof(buffer));
                ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
				std::cout << buffer << std::endl;
				std::cout << bytes_read << std::endl;
                if (bytes_read == -1) {
                    perror("Read failed");
                    close(server_fd);
                    close(kq);
                    exit(EXIT_FAILURE);
                }
                if (bytes_read == 0) {
                    // 클라이언트 연결 종료
                    std::cout << "Client disconnected" << std::endl;
                    close(events[i].ident);
                } else {
                    // 받은 데이터 출력
                    

                    std::string buf;
                    std::string result;
                    std::ifstream file;
                    file.open("index_copy.html", std::ifstream::in);
					// std::string a = "HTTP/1.1 200 OK\r\nContent-Length:5\r\n\r\nHELLO";

					// "HTTP/1.1 200 OK\r\nContent-Length:5\r\n\r\nHELLO";
                    result += "HTTP/1.1 200 OK\r\n";
                    result += "Content-Length:187\r\n";
                    result += "Content-Type: text/html\r\n";
					result += "Connection: close\r\n";
                    result += "Server: MyServer\r\n";
                    result += "\r\n";
                    while(true)
                    {
                        std::getline(file, buf);
						if (file.eof())
							break;
                        result += buf;
                    }
                    file.close();
					// std::cout << result << std::endl;
                    // bytes_read = ;
        			// send(client_fd, result.c_str(), result.length(), 0);
        			send(client_fd, result.c_str(), result.length(), 0);
                    // std::cout << "Received data from client: " << std::string(buffer, bytes_read) << std::endl;
                }
            }
        }
    }
    // 아 ~ 아 ~
	close(client_fd);
    close(server_fd);
    close(kq);
    return 0;
}
