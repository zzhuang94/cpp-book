#include <iostream>
#include <future>
#include <thread>

int multiply(int a, int b) { return a * b; }

int main() {
    // 1. 将普通函数包装成 packaged_task
    std::packaged_task<int(int, int)> task(multiply);
    
    // 2. 获取与该任务关联的 future
    std::future<int> result = task.get_future();
    
    // 3. 将任务交给另一个线程去执行 (或者放进线程池队列)
    std::thread t(std::move(task), 5, 6);
    
    // 4. 获取结果
    std::cout << "5 * 6 = " << result.get() << std::endl;
    
    t.join();
    return 0;
}