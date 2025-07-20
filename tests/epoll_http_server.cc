#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

const char* RESPONSE =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 12\r\n"
    "Connection: keep-alive\r\n"
    "\r\n"
    "Hello World!";

int set_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8020);

    bind(listen_fd, (sockaddr*)&addr, sizeof(addr));
    listen(listen_fd, 1024);
    set_nonblock(listen_fd);

    int epfd = epoll_create1(0);
    epoll_event ev, events[1024];
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    while (true) {
        int n = epoll_wait(epfd, events, 1024, -1);
        for (int i = 0; i < n; ++i) {
            int fd = events[i].data.fd;
            if (fd == listen_fd) {
                sockaddr_in cli_addr;
                socklen_t cli_len = sizeof(cli_addr);
                int conn_fd = accept(listen_fd, (sockaddr*)&cli_addr, &cli_len);
                set_nonblock(conn_fd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, conn_fd, &ev);
            } else {
                char buf[4096];
                int r = read(fd, buf, sizeof(buf));
                if (r > 0) {
                    // 简单判断GET请求
                    if (strncmp(buf, "GET", 3) == 0) {
                        write(fd, RESPONSE, strlen(RESPONSE));
                    }
                }
                // 关闭短连接，或可选支持 keep-alive
                // close(fd);
                // epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
            }
        }
    }
    close(listen_fd);
    return 0;
} 