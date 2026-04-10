#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <thread>

#define MAX_EVENTS 10
#define PORT 8083

// 设置文件描述符为非阻塞 (Non-blocking I/O)
void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// 模拟线程池中的工作线程处理业务
void workerThread(int client_fd) {
    char buffer[1024] = {0};
    // 非阻塞读取数据
    int bytes_read = read(client_fd, buffer, sizeof(buffer));
    if (bytes_read > 0) {
        std::cout << "[线程 " << std::this_thread::get_id() << "] 收到数据: " << buffer;
        // 处理完业务后，把数据原样发回去 (Echo)
        write(client_fd, buffer, bytes_read);
    } else if (bytes_read == 0) {
        std::cout << "客户端断开连接\n";
        close(client_fd);
    }
}

int main() {
    // 1. 创建监听 Socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setNonBlocking(server_fd); // 设置为非阻塞

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, SOMAXCONN);

    // 2. 创建 epoll 实例 (大堂经理)
    int epoll_fd = epoll_create1(0);
    struct epoll_event event, events[MAX_EVENTS];
    
    // 让 epoll 监听 server_fd 的“可读”事件 (有新客人来了)
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

    std::cout << "服务器启动，监听端口 " << PORT << "...\n";

    // 3. 事件循环 (大堂经理开始巡视)
    while (true) {
        // epoll_wait 会阻塞，直到有事件发生 (比如有数据来了，或者有新连接)
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == server_fd) {
                // 情况 A：监听 Socket 有事件，说明有【新客户端】连接
                sockaddr_in client_addr{};
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                
                setNonBlocking(client_fd); // 新连接也设置为非阻塞
                
                // 把新客人的 fd 也交给 epoll 监听
                event.events = EPOLLIN | EPOLLET; // 边缘触发模式
                event.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
                std::cout << "新客户端连接: fd " << client_fd << "\n";

            } else {
                // 情况 B：普通的客户端 Socket 有事件，说明【有数据发来了】
                int client_fd = events[i].data.fd;
                
                // 【核心结合点】：大堂经理拿到事件，不自己处理，扔给“线程池”去读写
                // 这里为了演示，直接起一个分离的线程代替线程池。
                // 在工业代码中，这里会把 client_fd 压入任务队列，由预先创建好的线程池去消费。
                std::thread(workerThread, client_fd).detach();
            }
        }
    }

    close(server_fd);
    return 0;
}
