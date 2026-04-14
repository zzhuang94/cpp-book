#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <chrono>

// ==========================================
// 1. 定义标准的工程化线程池 (常规用法)
// ==========================================
class ThreadPool {
public:
    ThreadPool(size_t num_threads) : stop(false) {
        // 启动指定数量的工作线程
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        // 加锁，等待任务到来或收到停止信号
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this] { 
                            return this->stop || !this->tasks.empty(); 
                        });
                        
                        // 如果收到停止信号且队列为空，线程退出
                        if (this->stop && this->tasks.empty()) {
                            return;
                        }
                        
                        // 取出任务
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    // 执行任务 (此时已解锁，不影响其他线程取任务)
                    task();
                }
            });
        }
    }

    // 提交任务到队列
    void enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace(std::move(task));
        }
        condition.notify_one(); // 唤醒一个空闲线程
    }

    // 析构函数：等待所有任务执行完毕并回收线程
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all(); // 唤醒所有阻塞的线程，让它们准备退出
        for (std::thread &worker : workers) {
            if (worker.joinable()) {
                worker.join(); // 阻塞主线程，直到工作线程执行完队列中的任务
            }
        }
    }

private:
    std::vector<std::thread> workers;        // 线程组
    std::queue<std::function<void()>> tasks; // 任务队列
    std::mutex queue_mutex;                  // 互斥锁
    std::condition_variable condition;       // 条件变量
    bool stop;                               // 停止标志
};

// ==========================================
// 2. 业务逻辑：1000个任务，20个线程，汇总结果
// ==========================================
int main() {
    const int NUM_TASKS = 1000;
    std::atomic<long long> total_sum{0}; // 线程安全的累加器

    {
        // 创建包含 20 个线程的线程池
        ThreadPool pool(20);

        // 投递 1000 个任务
        for (int i = 0; i < NUM_TASKS; ++i) {
            pool.enqueue([i, &total_sum]() {
                // 1. 模拟 IO 查询耗时 (例如 10 毫秒)
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                
                // 2. 模拟查询到的数字 (假设查到的就是 i 本身)
                int fetched_number = i;

                // 3. 汇总结果 (原子累加，无需额外加锁)
                total_sum += fetched_number; 
            });
        }
        
        std::cout << "所有任务已投递到线程池，等待执行..." << std::endl;
        
    } // <-- 关键点：离开作用域时，pool 的析构函数会被调用，主线程会在这里阻塞，直到所有任务执行完毕。

    // 输出最终结果
    std::cout << "所有任务执行完毕！" << std::endl;
    std::cout << "计算总和为: " << total_sum.load() << std::endl;

    return 0;
}