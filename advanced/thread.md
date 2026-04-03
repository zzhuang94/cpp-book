# 多线程

> `C++11` 是 `C++` 并发能力真正“进入标准库”的分水岭。

在这之前，多线程程序往往依赖平台 API（如 `pthread`、`Win32`），可移植性和学习成本都更高。  
`C++11` 把线程、锁、条件变量、原子、异步任务放进了统一体系，现代并发编程才有了共同语言。

这篇文档按“从能跑到写对，再到写稳”的顺序展开。你会看到：

- 线程如何创建与回收
- 共享数据为什么会出错
- 锁与条件变量如何正确配合
- 原子类型与内存序是什么边界
- 什么时候用 `thread`，什么时候用 `async`

----

# 最小示例 std::thread

```cpp
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
```

> 必须 `join()` 或 `detach()`: 

**`std::thread` 对象在析构前，如果仍然是 `joinable` 状态，程序会直接 `std::terminate`。**

```cpp
std::thread t(work);
if (t.joinable()) {
    t.join();
}
```

*初学阶段默认优先 `join()`。`detach()` 只在你非常确定“后台线程独立活着是安全的”时再用。*

----

# 线程参数与 lambda

线程入口可以是普通函数，也可以是 `lambda`，后者在局部任务里更常见：

```cpp
#include <iostream>
#include <string>
#include <thread>

void greet(const std::string& name, int id) {
    std::string message = "hello " + name + ", id=" + std::to_string(id) + "\n";
    std::cout << message;
}

int main() {
    std::thread t1(greet, "cpp11", 7);

    int local = 42;
    std::thread t2([local]() {
        std::string message = "captured local = " + std::to_string(local) + "\n";
        std::cout << message;
    });

    t1.join();
    t2.join();
    return 0;
}

```

注意两个常见坑：

- 引用捕获 `([&])` 必须保证被引用对象在线程结束前仍然有效
- 传引用参数时通常需要 `std::ref(obj)`，否则会发生拷贝

----

# 共享状态与数据竞争

> 多线程最难的点不是“创建线程”，而是“共享数据”。

只要满足下面条件，就可能出现数据竞争：
- 多个线程访问同一内存位置
- 至少一个线程在写
- 访问之间没有同步关系

在 `C++` 内存模型里，**数据竞争属于未定义行为（UB），不是“偶尔结果不对”这么简单。**

```cpp
#include <iostream>
#include <thread>

int counter = 0;

void add() {
    for (int i = 0; i < 100000; ++i) {
        ++counter; // 非原子复合操作
    }
}

int main() {
    std::thread t1(add);
    std::thread t2(add);
    t1.join();
    t2.join();
    std::cout << counter << "\n";  // 输出的结果小于期望的 200000
}
```

`++counter` 不是一个不可分割的机器动作，它通常包含读、改、写多个阶段。线程交错后，结果就会丢失更新。

----

# 互斥锁

系统提供两种互斥锁：
- `lock_guard`：轻量、语义直接，适合 `进作用域就加锁，出作用域就解锁`
- `unique_lock`：更灵活，支持延迟加锁、手动解锁、和条件变量配合

默认优先 `lock_guard`，需要灵活控制再升级到 `unique_lock`。

## lock_guard

> `std::mutex` + `std::lock_guard` 是最基础、最常用的保护方式

```cpp
#include <iostream>
#include <mutex>
#include <thread>

int counter = 0;
std::mutex mtx;

void add() {
    for (int i = 0; i < 100000; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        ++counter;
    }
}

int main() {
    std::thread t1(add);
    std::thread t2(add);
    t1.join();
    t2.join();
    std::cout << counter << "\n";
}
```

`lock_guard` 的本质是 RAII：**构造时加锁，析构时解锁**。这样即使中间 `return` 或异常，也不会漏解锁。

## uniuqe_lock

> `std::unique_lock` 是“功能更全的 RAII 锁包装器”，适合需要**更灵活锁控制**的场景。

你可以把它理解为：

- `lock_guard`：简单、轻量、几乎零心智负担
- `unique_lock`：稍重一些，但可以“晚点加锁 / 先尝试加锁 / 临时解锁再加回”

### 典型使用场景

- 需要 `defer_lock`（先构造对象，后续再决定何时 `lock()`）
- 需要 `try_lock` 逻辑（拿不到锁就走降级路径）
- 需要在一个作用域内临时 `unlock()`，做完非临界工作后再 `lock()`
- 需要与 `condition_variable::wait` 配合（`wait` 要求传 `unique_lock`）

### 常见构造方式

```cpp
std::mutex mtx;

// 1) 构造即加锁，离开作用域解锁；还可以手动解锁再次加锁加锁
std::unique_lock<std::mutex> lk1(mtx);
lock.unlock();  // 提前手动解锁！让其他线程可以进来了
do_something();
lock.lock();    // 如果后面还需要，可以再次加锁
// 离开作用域，如果当前是锁定状态，会自动解锁；如果已经解锁，则什么都不做。

// 2) defer_lock：延迟加锁，先创造一把锁，稍后加锁
std::unique_lock<std::mutex> lk2(mtx, std::defer_lock);
do_something();
lk2.lock();
// 同第一种用法类似，也可以无限次再解锁加锁

// 3) try_to_lock：尝试加锁，不阻塞等待
std::unique_lock<std::mutex> lk3(mtx, std::try_to_lock);
if (!lk3.owns_lock()) {
    // 没拿到锁，走备用逻辑
}
```

### 更完整示例：临时解锁缩短临界区

```cpp
#include <mutex>
#include <string>
#include <vector>
#include <thread>
#include <iostream>

std::mutex mtx;
std::vector<std::string> logs;

void writeLog(const std::string& msg) {
    for (int i = 0; i < 10; ++i) {
        std::unique_lock<std::mutex> lk(mtx);
        logs.push_back(msg);          // 临界区：修改共享容器

        lk.unlock();                  // 下面是较慢操作，不占用锁
        // 模拟较慢操作，随机sleep 1~100ms
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100 + 1));
        std::string formatted = "[LOG] " + msg + " " + std::to_string(i);

        lk.lock();                    // 需要再次访问共享数据时再加锁
        logs.push_back(formatted);
    }
}
int main() {
    std::thread t1(writeLog, "XXXXX");
    std::thread t2(writeLog, "-----");
    t1.join();
    t2.join();
    for (const auto& log : logs) {
        std::cout << log << std::endl;
    }
    return 0;
}

```

这个模式的核心是：把锁只包住“必须受保护”的最小区间，减少线程互相阻塞时间。

### 使用注意点

- `unique_lock` 可移动、不可拷贝；锁所有权是“唯一的”
- 手动 `unlock()` 后要明确后续是否仍需 `lock()`，避免逻辑遗漏
- `owns_lock()` 可用于判断当前是否持锁，重复加锁或解锁会抛出 `std::system_error` 异常导致程序崩溃
- 能用 `lock_guard` 的地方优先 `lock_guard`；只有需要控制力时再上 `unique_lock`

## 多把锁与死锁

> 在多线程编程中，如果需要同时锁定多个互斥锁，极易发生 `​​死锁（Deadlock）`

```cpp
// 线程 A: lock(m1) -> lock(m2)
// 线程 B: lock(m2) -> lock(m1)
```

工程里常见两种做法：

- 固定全局加锁顺序（例如总是先 `m1` 后 `m2`）
- 用 `std::lock(m1, m2)` 再配合 `adopt_lock`

### adopt_lock

`​std::lock(m1, m2, ...)​`​ 可以安全地同时锁定多个互斥锁，保证​​绝对不会发生死锁​​。  
`std::lock()` 只是负责加锁，​​它不具备 `RAII（自动解锁`）的功能​​。如果后面抛出异常，锁就释放不掉了。  
这时候，std::adopt_lock 就派上用场了！

```cpp
#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx1;
std::mutex mtx2;

void safe_transfer() {
    // 1. 使用全局的 std::lock 安全地同时锁定两个互斥锁（防死锁）
    // 此时 mtx1 和 mtx2 都已经被当前线程锁住了
    std::lock(mtx1, mtx2);

    // 2. 使用 adopt_lock 让 unique_lock（或 lock_guard）接管这两个锁
    // 告诉它们：“我已经锁好了，你们别再锁了，只负责离开作用域时帮我解锁！”
    std::unique_lock<std::mutex> lock1(mtx1, std::adopt_lock);
    std::unique_lock<std::mutex> lock2(mtx2, std::adopt_lock);

    // 3. 安全地操作共享数据...
    std::cout << "安全地同时操作两个受保护的资源..." << std::endl;

    // 4. 离开作用域，lock2 和 lock1 依次析构，自动调用 mtx2.unlock() 和 mtx1.unlock()
}

int main() {
    std::thread t1(safe_transfer);
    std::thread t2(safe_transfer);
    t1.join();
    t2.join();
    return 0;
}
```

----

# 条件变量

> `std::condition_variable（条件变量）` 是多线程编程中用于​​线程间同步​​的核心工具。

如果说 `std::mutex` 是为了防止多个线程同时抢夺资源， **互斥**。  
那么 `std::condition_variable` 就是为了实现​​“等待-通知”机制，**协作**​。

## 使用场景

假设线程 A 需要等待某个条件成立（比如“队列里有数据了”）才能继续执行。

- **错误做法（轮询/忙等待）**​​： 写一个死循环一直检查，这会疯狂消耗 CPU 资源（CPU 占用率飙升到 100%）。
- **​正确做法（条件变量）**​​： 线程 A 让自己​​进入休眠状态（不消耗 CPU）​​，线程 B 把数据放进队列后​唤醒​​线程 A。

## 核心三要素

要使用条件变量，你​​必须​​同时具备以下三个要素：

- **​共享状态（条件）**​​：比如一个 bool ready 标志，或者一个 std::queue。
- **​互斥锁（std::mutex）**​​：保护共享状态不被并发破坏。
- **​条件变量（std::condition_variable）**​​：负责阻塞和唤醒线程。

## 经典场景

生产者-消费者模型，这是条件变量最标准、最常用的代码模板：

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

std::mutex mtx;
std::condition_variable cv;
std::queue<int> data_queue; // 共享数据
bool finished = false;      // 结束标志

// 消费者线程
void consumer(std::string name) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        
        // 核心：wait() 的用法
        // 1. 如果 lambda 返回 false，wait 会自动解锁 mtx，并让当前线程休眠。
        // 2. 当被 notify 唤醒时，wait 会自动重新加锁 mtx，并再次检查 lambda。
        // 3. 如果 lambda 返回 true，wait 结束，线程继续往下执行。
        cv.wait(lock, [] { 
            return !data_queue.empty() || finished; 
        });

        // 如果队列为空且生产者已经结束，则退出循环
        if (data_queue.empty() && finished) {
            break;
        }

        // 取出数据并处理
        int data = data_queue.front();
        data_queue.pop();
        std::cout << name << " 消费者处理了数据: " << data << std::endl;

        // 离开作用域，自动解锁
    } 
}

// 生产者线程
void producer(std::string name) {
    for (int i = 1; i <= 10; ++i) {
        {
            std::unique_lock<std::mutex> lock(mtx);
            data_queue.push(i);
            std::cout << name << " 生产者生产了数据: " << i << std::endl;
        } // 生产完毕，提前解锁，让消费者能尽快拿到锁
        
        cv.notify_one(); // 唤醒一个正在 wait 的消费者线程
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 模拟耗时
    }

    // 生产结束，设置标志并通知消费者退出
    {
        std::unique_lock<std::mutex> lock(mtx);
        finished = true;
    }
    cv.notify_all(); // 唤醒所有可能还在等待的消费者
}

int main() {
    std::thread t1(consumer, "消费者1");
    std::thread t2(producer, "生产者1");
    std::thread t3(consumer, "消费者2");

    t1.join();
    t2.join();
    t3.join();
    return 0;
}

```

> `std::condition_variable` 的 `wait` 方法​​强制要求​​传入 `unique_lock`，不能用 `lock_guard`。

## 核心API

### wait(lock, predicate)

- **lock**：一个 `std::unique_lock` 对象
- **predicate**：一个返回 bool 的可调用对象（通常是 Lambda 表达式）

> 操作系统底层存在一种现象叫​ `虚假唤醒（Spurious Wakeup）`​，即线程可能在没有收到 `notify` 的情况下莫名其妙地醒来。

如果醒来时条件并不满足，直接往下执行会导致严重 Bug。  
传入 Lambda 后，wait 内部相当于做了一个安全的 while 循环：

```cpp
// cv.wait(lock, []{ return condition; }) 的底层等价逻辑：
while (!condition()) {
    cv.wait(lock); // 休眠并解锁，醒来后重新加锁
}
```

### notify_one()

唤醒​​一个​​正在等待的线程。如果有多个线程在等，操作系统随机挑一个唤醒。  
适用于“一份数据只能被一个消费者处理”的场景（如任务队列）。

### notify_all()

唤醒​​所有​​正在等待的线程。  
适用于“状态广播”的场景（比如上面的例子中，通知所有消费者“生产结束了，大家都可以下班了”）。


----

# 原子类型

对简单计数、标志位，`std::atomic` 很实用：

```cpp
#include <atomic>
#include <iostream>
#include <thread>

std::atomic<int> counter(0);

void add() {
    for (int i = 0; i < 100000; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    std::thread t1(add);
    std::thread t2(add);
    t1.join();
    t2.join();
    std::cout << counter.load() << "\n";
}
```

这里使用 `memory_order_relaxed`，表示只要这次原子读写本身是原子的，不额外建立跨线程顺序约束。

原子不能替代所有锁，一旦你需要：
- 多个变量保持一致性
- 临界区里有复杂业务逻辑
- 条件等待与状态转换协同

就应该回到“锁 + 条件变量”模型。

> 原子适合做小而明确的同步点，不适合承载复杂事务逻辑。

## 内存序（memory order）

`C++11` 提供多种内存序，实际学习可以先分三层：

- `memory_order_relaxed`：只保证原子性，不保证顺序传播
- `memory_order_acquire` / `memory_order_release`：建立“发布-获取”同步
- `memory_order_seq_cst`：最强、最易理解、也最保守

教学上的实用建议：

- 不清楚时先用默认 `seq_cst` 写对
- 明确是“纯计数器”再考虑 `relaxed`
- 只有在你能证明同步关系时才精细调优

内存序是并发里最容易“看起来懂、实际误用”的部分，建议先建立正确性，再优化。

> *在 `C++` 多线程编程中，​​99% 的情况下你都不需要用到 `内存序（Memory Order）`​​。*  
> *只要你老老实实地使用 `std::mutex` 和 `std::condition_variable`，标准库已经在底层帮你处理好了一切。*

----

# 任务化并发

> ​核心思想： ​将程序员的注意力从 `管理线程（Thread）` 转移到 `管理任务（Task）` 和 `返回值` 上。​

传统的 `std::thread` 就像是雇佣了一个工人，你必须自己管理他的生死，  
而且很难直接从他手里拿到计算结果（通常需要借助全局变量和互斥锁）。

而任务化并发就像是​​ *外包*​​：你提交一个任务，立刻拿到一张 *提货单*，等你需要结果时，凭提货单去取即可。

`C++11` 提供了三个核心工具来实现任务化并发，它们都围绕着一个核心概念：​`​std::future（未来量/提货单）`​​。

## std::future

> `std::future` 是一个占位符。它表示一个​​ *目前可能还不存在，但在未来某个时刻会准备好的值（或异常）*。  
> 你不能直接向 future 里写数据，你只能从中​​读取​​数据（通过 get() 方法）。  

那么，谁来向 future 里写数据呢？C++11 提供了三种方式：

### 最简单的高级接口 std::async

这是日常开发中最常用的任务化工具。它自动帮你创建线程（或复用线程），执行函数，并将结果绑定到一个 `std::future 上`。

```cpp
#include <iostream>
#include <future>
#include <chrono>
#include <thread> 

// 一个耗时的计算任务
int calculate_sum(int a, int b) {
    std::this_thread::sleep_for(std::chrono::seconds(2)); // 模拟耗时
    return a + b;
}

int main() {
    std::cout << "1. 提交任务..." << std::endl;
    
    // 使用 std::async 启动异步任务，立刻返回一个 future (提货单)
    // std::launch::async 强制要求在后台开启新线程执行
    std::future<int> result_future = std::async(std::launch::async, calculate_sum, 10, 20);
    
    std::cout << "2. 主线程可以继续做其他事情..." << std::endl;
    
    // 当我们需要结果时，调用 get()
    // 注意：如果后台任务还没执行完，get() 会阻塞当前线程直到结果准备好！
    int final_result = result_future.get(); 
    
    std::cout << "3. 拿到结果: " << final_result << std::endl;
    return 0;
}
```

> std::async 的两种启动策略（Launch Policy）：​
> - std::launch::async：​​强制​​异步。必须开启一个新线程去执行任务。
> - std::launch::deferred：​​延迟（惰性）​​执行。不开启新线程，直到你调用 future.get() 时，才在​​当前线程​​中同步执行该任务。

### 底层的通信通道 std::promise

如果说 `std::async` 是全自动的，那么 `std::promise` 就是​​手动挡​​。

`std::promise` 和 `std::future` 是一对绑定的搭档。promise 负责​​生产（写入）​​值，future 负责​​消费（读取）​​值。  
它们可以在不同的线程中安全地传递数据，而不需要你手写互斥锁。

> 适用场景：​ 当你不想让 C++ 帮你管理线程，而是想自己控制线程，但又需要优雅地传递返回值时。

```cpp
#include <iostream>
#include <thread>
#include <future>
#include <thread>

// 工作线程：持有 promise，负责计算并填入结果
void worker(std::promise<int> prom) {
    std::cout << "后台线程正在计算..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    try {
        int result = 42; // 假设这是复杂的计算结果
        std::cout << "后台线程计算结果加入 promise: " << result << std::endl;
        prom.set_value(result); // 将结果填入 promise，此时关联的 future 就会就绪
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "后台线程计算结果加入 promise 完成" << std::endl;
    } catch (...) {
        prom.set_exception(std::current_exception()); // 甚至可以跨线程传递异常！
    }
}

int main() {
    // 1. 创建一个 promise 对象
    std::promise<int> my_promise;
    
    // 2. 从 promise 中获取对应的 future (提货单)
    std::future<int> my_future = my_promise.get_future();
    
    // 3. 启动线程，把 promise 移动给后台线程 (promise 不能拷贝，只能 move)
    std::thread t(worker, std::move(my_promise));
    
    // 4. 主线程等待并获取结果
    std::cout << "主线程开始等待结果: " << std::endl;
    auto result = my_future.get();
    std::cout << "主线程获取到结果: " << result << std::endl;
    
    t.join();
    return 0;
}
```

### 打包任务 std::packaged_task

`std::packaged_task` 可以把任何可调用对象（普通函数、Lambda 表达式等）包装起来，使其具备​​ *执行后自动将返回值存入 future”* ​的能力。

> 它是实现​​线程池（Thread Pool）​​的绝佳利器。

可以把一堆 packaged_task 塞进任务队列里，让线程池里的线程去无脑执行它们（调用 task()），  
而提交任务的业务层可以通过 future 拿到各自的结果。

```cpp
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
```

## 如何选择

- **​首选 std::async**​​：如果你只是想简单地在后台跑一个函数并拿到结果，用它最省事。
- **​造轮子用 std::packaged_task**​​：如果你在写底层的任务调度器或线程池，用它来封装任务。
- **​复杂通信用 std::promise**​​：如果你需要在某个特定时刻（而不是函数结束时）传递一个值，或者需要跨线程传递异常，用它。

----

# 异常与线程边界

> 一个常被忽略的事实：线程函数里抛出的异常不会自动传播到创建线程的调用栈。

常见处理思路：

- 在线程函数内部捕获并记录错误
- 或使用 `std::promise` / `std::future` 把异常传回主线程（`set_exception`）
- 或改用 `std::async`，让异常随 `future::get()` 传播

并发代码要把“错误如何上报”当成设计的一部分，而不是最后补丁。

----

# 常见误区

### 误区 1：`volatile` 可以线程同步

不对。`volatile` 主要用于特殊内存访问语义（如硬件寄存器场景），不是并发同步原语。

### 误区 2：`detach()` 更高级

不对。`detach()` 只是把生命周期管理变难。没有强理由时，优先 `join()`。

### 误区 3：只要用了锁就绝对安全

不对。还有死锁、锁顺序不一致、锁粒度过粗、条件变量误用等问题。

### 误区 4：`sleep_for` 能做同步

不对。`sleep` 只能“等待时间”，不能“等待条件成立”。

### 误区 5：原子一定比锁快

不对。性能取决于争用、缓存一致性开销、代码结构与硬件。先写对，再测量。

----

# 推荐实践

- 先画出共享状态，再写线程；先定义数据所有权，再写锁。
- 默认使用 RAII 锁（`lock_guard` / `unique_lock`），避免裸 `lock()/unlock()`。
- 临界区尽量小，但不要为了“缩小”而破坏一致性。
- 条件变量等待必须带谓词，防止虚假唤醒。
- 多把锁必须有固定顺序，或统一用 `std::lock`。
- 原子只解决局部同步点，复杂状态机仍用锁和条件变量。
- 并发代码务必准备可复现测试（高循环次数、压力测试、日志采样）。

----

# 小结

`C++11` 多线程编程可以浓缩成一句话：**线程很容易创建，正确同步才是核心成本。**

从学习顺序看，最稳妥的路径是：

- 先掌握 `thread + mutex + lock_guard`
- 再掌握 `condition_variable` 的等待模型
- 再理解 `atomic` 和内存序边界
- 最后根据业务选择 `thread` 或 `async/future`

当你开始用“所有权、同步关系、生命周期”去思考并发，而不只是“我开了几个线程”，你就真正进入了现代 `C++` 并发编程的门槛。
