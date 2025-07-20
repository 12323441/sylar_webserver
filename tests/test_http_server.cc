#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <vector>

#define PORT 8020
#define MAX_EVENTS 1024

const char* http_response =
    "HTTP/1.1 200 OK\r\n"
    "Content-Length: 12\r\n"
    "Connection: close\r\n"
    "\r\n"
    "Hello World\n";

int set_non_blocking(int fd) {
    int old = fcntl(fd, F_GETFL);
    return fcntl(fd, F_SETFL, old | O_NONBLOCK);
}

int create_listen_socket() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(listen_fd, 128) < 0) {
        perror("listen");
        exit(1);
    }

    set_non_blocking(listen_fd);
    return listen_fd;
}

int main() {
    int listen_fd = create_listen_socket();
    int epoll_fd = epoll_create(1);

    epoll_event ev{}, events[MAX_EVENTS];
    ev.data.fd = listen_fd;
    ev.events = EPOLLIN | EPOLLET; // 边缘触发
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

    std::cout << "HTTP Server running on port " << PORT << "..." << std::endl;

    while (true) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;

            if (fd == listen_fd) {
                // 接收所有连接
                while (true) {
                    sockaddr_in client{};
                    socklen_t len = sizeof(client);
                    int conn_fd = accept(listen_fd, (sockaddr*)&client, &len);
                    if (conn_fd < 0) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                        perror("accept");
                        break;
                    }
                    set_non_blocking(conn_fd);

                    epoll_event conn_ev{};
                    conn_ev.data.fd = conn_fd;
                    conn_ev.events = EPOLLIN | EPOLLET;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &conn_ev);
                }
            } else if (events[i].events & EPOLLIN) {
                char buffer[1024];
                while (true) {
                    int n = recv(fd, buffer, sizeof(buffer), 0);
                    if (n <= 0) {
                        close(fd);
                        break;
                    }
                    send(fd, http_response, strlen(http_response), 0);
                    close(fd); // 短连接
                    break;
                }
            }
        }
    }

    close(listen_fd);
    return 0;
}

