# 时间/日期

如果你以前学的是 `C++98`，那时处理时间通常依赖 C 风格接口，比如 `time()`、`clock()`、`tm`、`strftime()`。这些接口今天仍然可用，但 `C++11` 最大的升级是标准库引入了 `chrono`，让时间表达更类型安全、更适合性能测量。

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

这类接口简单直接，但存在几个问题：

- 单位和语义不够明确
- 容易混淆秒、毫秒、时钟滴答
- 类型系统帮助不够

# `std::chrono`

`C++11` 提供了 `std::chrono`，核心概念包括：

- `duration`：时间段
- `time_point`：时间点
- `clock`：时钟

最常见的是：

- `std::chrono::system_clock`
- `std::chrono::steady_clock`
- `std::chrono::high_resolution_clock`

# 测量耗时

```cpp
#include <chrono>
#include <iostream>

int main() {
    auto start = std::chrono::steady_clock::now();

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

# `steady_clock`

做性能测量时，通常更推荐 `steady_clock`，因为它是单调时钟，不会因为系统时间调整而跳变。

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

# 睡眠

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

# 推荐实践

- 计时优先使用 `std::chrono`，不要直接拿整数表示时间。
- 性能测量优先使用 `steady_clock`。
- 与旧接口交互时，可以在 `chrono` 和 `time_t` 之间做转换。
- 时间单位要显式写出来，避免“这个 1000 到底是秒还是毫秒”的歧义。

# 小结

`C++11` 之前，时间处理更多依赖 C 风格 API 和经验约定；`C++11` 之后，`chrono` 把时间语义显式建模成类型，显著提升了时间处理的可读性和安全性。

## 这章最值得建立的直觉

在很多其它语言里，时间值经常被表示成：

- 整数时间戳
- 某个统一日期对象
- 毫秒数或秒数

而 `C++11` 的 `chrono` 更强调：

- 时间点是时间点
- 时长是时长
- 单位应该体现在类型里

这会让代码读起来更啰嗦一点，但也更不容易把“秒、毫秒、微秒”混在一起。

## `steady_clock` 和 `system_clock` 别混用心智

一个非常重要的第一印象是：

- `system_clock` 更接近现实世界时间
- `steady_clock` 更适合测量经过的时间

如果你做性能计时、超时控制或间隔测量，通常更应该想到 `steady_clock`，因为它不容易受到系统时间调整影响。

## 时间单位不要再靠注释猜

例如下面这种旧式思路：

```cpp
int timeout = 1000; // 毫秒
```

在 `chrono` 风格下，更推荐：

```cpp
auto timeout = std::chrono::milliseconds(1000);
```

这样单位直接进入类型，阅读和维护都更稳。

## 常见误区

### 误区 1：时间就是一个整数

不对。现代 `C++` 更鼓励把时间单位写进类型。

### 误区 2：做耗时统计时任何时钟都一样

不对。`steady_clock` 和 `system_clock` 适用场景不同。

### 误区 3：`chrono` 太重，不如直接用整数简单

短期看可能更短，长期看很容易丢失单位语义。

## 补充建议

- 性能测量优先 `steady_clock`。
- 时间长度优先使用 `duration` 类型，而不是裸整数。
- 与旧接口交互时再转换为 `time_t` 或数值。
- 看到裸时间整数时，主动问自己单位到底是什么。
