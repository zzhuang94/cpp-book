#include <iostream>
#include <thread>

void work() {
    std::cout << "work in child thread\n";
}

int main() {
    std::thread t(work);    // 创建线程并启动
    t.join();               // 等待线程执行完成
    return 0;
}
