# 并发基础

如果你来自 `Go`、`JS`、`TS` 等语言，学习 `C++11` 并发时最大的困难往往不是“线程是什么”，而是：

- `C++` 没有默认帮你处理共享状态安全
- 多线程下的数据竞争会直接变成未定义行为
- 锁、线程、条件变量、原子类型都需要更显式地使用

好消息是，`C++11` 第一次把线程库正式纳入标准库，这使得现代并发基础至少有了统一入口。

## `C++11` 并发里有哪些核心工具

最常见的一组工具包括：

- `std::thread`
- `std::mutex`
- `std::lock_guard`
- `std::unique_lock`
- `std::condition_variable`
- `std::atomic`

在基础阶段，你至少要先理解线程、互斥锁和数据竞争。

## 创建线程

最简单的线程示例：

```cpp
#include <iostream>
#include <thread>

void work() {
    std::cout << "work in thread" << std::endl;
}

int main() {
    std::thread t(work);
    t.join();
    return 0;
}
```

这里：

- `std::thread t(work);` 创建一个线程
- `t.join();` 等待线程执行完毕

## 为什么必须 `join` 或 `detach`

线程对象销毁前，必须处理其执行状态。最常见的方式是：

- `join()`：等待线程结束
- `detach()`：让线程在后台独立运行

对初学者来说，更推荐优先使用 `join()`，因为 `detach()` 更容易带来生命周期和同步问题。

## 用 lambda 启动线程

现代 `C++11` 里，线程经常和 `lambda` 配合：

```cpp
#include <iostream>
#include <thread>

int main() {
    std::thread t([]() {
        std::cout << "hello from lambda thread" << std::endl;
    });

    t.join();
    return 0;
}
```

这在写临时任务或小范围并发逻辑时很方便。

## 数据竞争是什么

多个线程同时访问同一份数据，并且至少有一个线程在写，而这些访问又没有正确同步，就可能产生数据竞争。

在 `C++` 里，数据竞争不是“偶尔有 bug”那么简单，它属于非常严重的问题，因为它会进入未定义行为。

## 一个不安全例子

```cpp
#include <iostream>
#include <thread>

int counter = 0;

void increase() {
    for (int i = 0; i < 100000; ++i) {
        ++counter;
    }
}

int main() {
    std::thread t1(increase);
    std::thread t2(increase);

    t1.join();
    t2.join();

    std::cout << counter << std::endl;
    return 0;
}
```

这段代码看起来很简单，但 `counter` 的并发写入没有同步，结果不可靠。

## 使用互斥锁保护共享数据

最基础的同步方式是 `std::mutex`：

```cpp
#include <iostream>
#include <mutex>
#include <thread>

int counter = 0;
std::mutex counterMutex;

void increase() {
    for (int i = 0; i < 100000; ++i) {
        std::lock_guard<std::mutex> lock(counterMutex);
        ++counter;
    }
}

int main() {
    std::thread t1(increase);
    std::thread t2(increase);

    t1.join();
    t2.join();

    std::cout << counter << std::endl;
    return 0;
}
```

`std::lock_guard` 的价值在于：

- 进入作用域时加锁
- 离开作用域时自动解锁

这又一次体现了 `RAII` 思想。

## 为什么不要手动 `lock` / `unlock` 当默认写法

看起来你也可以这样写：

```cpp
counterMutex.lock();
++counter;
counterMutex.unlock();
```

但这种方式一旦中途 `return`、抛异常或逻辑变复杂，就容易漏解锁。更推荐把锁交给 `lock_guard` 或 `unique_lock` 这样的对象管理。

## `std::atomic`

对于非常简单的共享计数场景，`std::atomic` 常常更直接：

```cpp
#include <atomic>
#include <iostream>
#include <thread>

std::atomic<int> counter(0);

void increase() {
    for (int i = 0; i < 100000; ++i) {
        ++counter;
    }
}
```

不过要注意，原子类型不是“万能替代锁”。一旦涉及多个变量之间的一致性、复杂临界区或复合逻辑，锁仍然是常见选择。

## 线程参数传递

线程函数也能接收参数：

```cpp
#include <iostream>
#include <string>
#include <thread>

void greet(const std::string& name) {
    std::cout << "hello, " << name << std::endl;
}

int main() {
    std::thread t(greet, "cpp11");
    t.join();
    return 0;
}
```

当涉及引用传参时要更小心，因为线程和主线程的生命周期可能不同。

## `sleep_for`

线程库常和 `chrono` 配合：

```cpp
#include <chrono>
#include <iostream>
#include <thread>

int main() {
    std::cout << "wait..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "done" << std::endl;
    return 0;
}
```

这在演示、节流和测试中很常见，但不要把 `sleep` 当成可靠同步机制。

## 条件变量的存在意义

当一个线程需要等待另一个线程满足某个条件时，`condition_variable` 很有用。基础阶段你只需要先知道它用于：

- 等待某个状态变化
- 避免忙等循环

它比一味 `sleep_for` + 轮询更合适。

## 和其它语言做个对比

如果你来自其它语言，可以先这样理解：

- `Go` 常鼓励通过 channel 和 goroutine 组织并发。
- `JS/TS` 常见的是事件循环、Promise、异步回调，不默认共享多线程内存模型。
- `C++11` 的线程模型更直接接近操作系统线程，也更要求你显式保护共享状态。

这意味着 `C++` 并发的自由度更高，但踩坑成本也更高。

## 常见误区

### 误区 1：`volatile` 可以做线程同步

不对。`volatile` 不是并发同步工具，线程同步应使用互斥锁、原子类型等机制。

### 误区 2：只要是简单整数，加减就天然线程安全

不对。没有同步的并发读写仍然可能产生数据竞争。

### 误区 3：`detach()` 比 `join()` 更现代

不对。`detach()` 只是另一种生命周期选择，通常更难推理。

### 误区 4：用了锁就一定没有并发问题

不对。还可能出现死锁、锁粒度过粗、性能瓶颈和逻辑错误。

## 推荐实践

- 基础阶段优先学会 `thread + mutex + lock_guard` 这一套。
- 共享数据一旦可能被多个线程同时读写，就先考虑同步方案。
- 锁对象优先交给 RAII 包装类管理。
- 不要用 `sleep_for` 伪装真正的同步。
- 没有明确需求时，不要过早把简单程序改成多线程。

## 小结

`C++11` 并发的关键不只是“会不会创建线程”，而是能不能正确理解共享状态、同步和生命周期。对其它语言开发者来说，这一章最重要的收获应该是：在 `C++` 里，多线程不是默认安全的，你必须显式地把正确的同步关系写进代码里。
