# 时间/日期

- `C++98` 处理时间通常依赖 C 风格接口，比如 `time()`、`clock()`、`tm`、`strftime()`。
- 这些接口今天仍然可用，但 `C++11` 最大的升级是标准库引入了 `chrono`，让时间表达更类型安全、更适合性能测量。

----

# C 风格接口

```cpp
#include <ctime>
#include <iostream>

int main() {
    std::time_t now = std::time(nullptr);
    std::cout << now << std::endl;
    return 0;
}
```

`std::time()` 返回的是当前日历时间，对应的类型是 `std::time_t`。

- 可以先把 `time_t` 粗略理解成“时间戳”
- 它通常表示从 Unix 纪元 `1970-01-01 00:00:00 UTC` 到现在经过的秒数
- 这个类型本身适合保存、比较、做差值，但直接打印时可读性一般

如果想把它变成更容易读的文本，最常见的老办法就是 `std::ctime()`：

```cpp
#include <ctime>
#include <iostream>

int main() {
    std::time_t now = std::time(nullptr);

    std::cout << "timestamp: " << now << std::endl;
    std::cout << "text: " << std::ctime(&now);
    return 0;
}
```

这里要注意两点：

- `ctime()` 的参数是 `time_t*`，所以要写成 `&now`
- `ctime()` 返回 C 风格字符串，里面通常自带结尾换行

先建立这个直觉很重要：

- `time_t` 更像“适合计算和传递的时间值”
- `ctime()` 更像“把这个时间值转成人类可读文本”

后面如果看到两个时间点做差，通常就是两个 `time_t` 或两个 `time_point` 在求“间隔了多久”。

这类接口简单直接，但存在几个问题：

- 单位和语义不够明确
- 容易混淆秒、毫秒、时钟滴答
- 类型系统帮助不够

----

# std::chrono

`C++11` 提供了 `std::chrono`，核心概念包括：

- `duration`：时间段
- `time_point`：时间点
- `clock`：时钟

最常见的是：

- `std::chrono::system_clock`
- `std::chrono::steady_clock`
- `std::chrono::high_resolution_clock`

这几个名字初看会有点抽象，但可以先记一个最实用的判断：

- 想获取 **时间戳**，优先想到 `system_clock`
- 想测量 **耗时**，优先想到 `steady_clock`

----

# 测量耗时

```cpp
#include <chrono>
#include <iostream>

int main() {
    auto start = std::chrono::steady_clock::now();

    // volatile 告诉编译器：这个变量的值可能被程序之外的未知因素（如硬件、信号处理程序、另一个线程）改变，
    // 因此编译器不能对它进行任何优化（如省略、重排、缓存到寄存器等）
    // 防止编译器优化掉看似“无用”的循环。
    volatile long long sum = 0;
    for (int i = 0; i < 1000000; ++i) {
        sum += i;
    }

    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << diff.count() << " ms" << std::endl;
    return 0;
}
```

这里有几个重点：

- `now()` 取得当前时间点
- 时间点相减得到 `duration`
- `duration_cast` 做单位转换
- `count()` 取得数值

## steady_clock

做性能测量时，通常更推荐 `steady_clock`，因为它是单调时钟，不会因为系统时间调整而跳变。

----

# 时长类型

```cpp
#include <chrono>
#include <iostream>

int main() {
    std::chrono::seconds s(2);
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(s);

    std::cout << ms.count() << std::endl;
    return 0;
}
```

这比单纯使用整数表达“时间长度”更安全，因为单位被写进了类型。

你可以把它理解成：以前靠注释约定单位，现在改成让类型系统帮你表达单位。

----

# 当前时间

```cpp
#include <chrono>
#include <ctime>
#include <iostream>

int main() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);

    std::cout << std::ctime(&t);
    return 0;
}
```

这里把 `chrono` 时间点转回 `time_t`，再借助传统接口格式化输出。

这也是一种很常见的写法：内部用 `chrono` 保持类型安全，到了要和旧 API、日志格式或平台接口打交道时，再转换成传统类型。

----

# sleep

`C++11` 线程库也能配合 `chrono` 使用：

```cpp
#include <chrono>
#include <iostream>
#include <thread>

int main() {
    std::cout << "wait..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "done" << std::endl;
    return 0;
}
```

这说明 `chrono` 不只是“计时库”，它还是很多并发接口的统一时间表达方式。后面学线程、锁和超时控制时，你会反复遇到它。

----

# 推荐实践

- 计时优先使用 `std::chrono`，不要直接拿整数表示时间。
- 性能测量优先使用 `steady_clock`。
- 与旧接口交互时，可以在 `chrono` 和 `time_t` 之间做转换。
- 时间单位要显式写出来，避免“这个 1000 到底是秒还是毫秒”的歧义。

# 小结

这章真正要建立的判断，不是“怎么调用一个时间 API”，而是“这里表示的是时间点、时间长度，还是一次耗时测量”。

- 经过多久，优先想到 `duration`
- 测量耗时，优先想到 `steady_clock`
- 获取现实世界时间，再考虑 `system_clock`

只要把“单位写进类型、时钟按场景选”这两个习惯养起来，时间相关代码就会比直接堆整数稳得多。
