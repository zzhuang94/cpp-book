# 修饰符

这里的“修饰符”主要指那些会改变类型、对象生命周期、链接属性或语义边界的关键字与限定符。对于重新上手 `C++` 的开发者来说，这部分最容易“看得懂代码，但说不清为什么这么写”。

# 类型修饰符

基础类型前常见的修饰符有：

- `signed`
- `unsigned`
- `short`
- `long`
- `const`
- `volatile`

示例：

```cpp
unsigned int count = 10;
long long total = 10000000000LL;
const double pi = 3.14159;
```

# `signed` 与 `unsigned`

```cpp
#include <iostream>

int main() {
    signed int a = -5;
    unsigned int b = 5;

    std::cout << a << ", " << b << std::endl;
    return 0;
}
```

在工程代码里，`unsigned` 往往和大小、索引、位字段绑定，但它也会制造很多隐式转换问题，因此不能因为“不会小于 0”就机械使用。

# `short`、`long`、`long long`

这些修饰符主要影响取值范围。`C++11` 时代，`long long` 已经是很常用的大整数类型。

```cpp
long long id = 9223372036854775807LL;
```

# `const`

`const` 修饰“不能修改”，是最常用的语义修饰符之一。

```cpp
const int maxRetry = 3;
```

它还会影响函数参数、成员函数、指针、引用等语义，后续章节会详细展开。

# `volatile`

`volatile` 表示对象的值可能在程序控制之外发生变化，例如硬件寄存器或某些极特殊并发场景。

```cpp
volatile int* status = nullptr;
```

但要注意：

> `volatile` 不是线程同步工具。在现代并发程序里，线程间可见性和原子性应优先使用 `std::atomic`、互斥锁等机制。

# `static`

`static` 有多种含义，取决于位置。

局部变量中：

```cpp
void nextId() {
    static int id = 0;
    ++id;
}
```

它表示局部静态存储期，只初始化一次。

类成员中：

```cpp
class Counter {
public:
    static int total;
};
```

表示这个成员属于类本身，而不是某个对象实例。

# `extern`

`extern` 用于声明“这个变量或函数定义在别处”：

```cpp
extern int globalValue;
```

这在多文件项目里非常常见，尤其是头文件声明、源文件定义的配合。

# `mutable`

`mutable` 是很多老 C++ 程序员容易忽视的修饰符。它允许某个成员在 `const` 成员函数里仍可修改。

```cpp
class Cache {
public:
    int get() const {
        ++hitCount_;
        return value_;
    }

private:
    int value_ = 10;
    mutable int hitCount_ = 0;
};
```

这个特性常用于缓存、统计计数等“逻辑常量但物理可变”的场景。

# 综合示例

```cpp
#include <iostream>

class Counter {
public:
    Counter() { ++total_; }

    static int total() {
        return total_;
    }

private:
    static int total_;
};

int Counter::total_ = 0;

int main() {
    const unsigned int maxUsers = 100;
    Counter a;
    Counter b;

    std::cout << "maxUsers = " << maxUsers << std::endl;
    std::cout << "Counter::total() = " << Counter::total() << std::endl;
    return 0;
}
```

# 推荐实践

- `const` 尽量多用，让接口语义更明确。
- 对整数范围有要求时再选择 `short`、`long`、`long long`，不要靠习惯。
- 谨慎使用 `unsigned`，特别是和有符号整型混合比较时。
- 不要把 `volatile` 当成并发同步手段。
- 需要表达类级别共享状态时使用 `static` 成员。

# 小结

修饰符的价值在于“把附加语义写进类型和声明”。在 `C++11` 代码中，好的修饰符使用习惯能显著提升接口可读性、限制错误调用，并减少很多模糊地带。

## 为什么这章容易“看懂但没形成判断”

很多修饰符你可能早就见过，但真实项目里更难的是判断：

- 这里为什么要加 `static`
- 这里的 `extern` 是不是跨文件共享状态
- 这里的 `mutable` 到底是不是在破坏 `const`
- 这里用 `unsigned` 是合理还是历史包袱

所以这章的重点不是死记关键字，而是学会从声明中读出额外语义。

## `static` 在不同位置含义不同

初学者最容易混淆的就是 `static`。你至少要形成这样的第一反应：

- 在局部变量上，表示静态存储期
- 在类成员上，表示属于类本身

同一个关键字在不同位置承担不同角色，这是 `C++` 很典型的阅读难点之一。

## `extern` 帮你识别“定义在别处”

当你在头文件里看到：

```cpp
extern int g_retryCount;
```

要立刻意识到：

- 这里不是定义
- 真正存储空间在别的源文件里
- 这通常和跨文件共享状态有关

这对于理解多文件项目特别重要。

## `volatile` 最需要记住的是“别滥用”

很多初学者会以为它和并发有关，于是把它当作线程同步工具。但现代 `C++` 中更稳妥的结论是：

> `volatile` 不是线程同步原语。

如果你处理的是并发共享数据，更应该想到的是：

- `std::mutex`
- `std::lock_guard`
- `std::atomic`

## `mutable` 的存在说明 `const` 也有边界

`mutable` 并不是鼓励你随便绕开 `const`，它的意义更接近：

- 某些成员属于实现细节
- 从对象的逻辑语义看，它们的变化不算“真正修改对象”

比如缓存命中次数、延迟计算结果、调试统计信息等。

## 常见误区

### 误区 1：`unsigned` 一定比 `int` 更安全

不对。它会引入不少隐式转换陷阱。

### 误区 2：`volatile` 可以替代锁

不对。线程同步请使用并发库工具。

### 误区 3：`static` 只有一种含义

不对。它强烈依赖出现位置。

## 补充建议

- 读声明时主动问：这个修饰符增加了什么语义。
- 遇到 `static` 时先判断它处于局部、类成员还是别的上下文。
- `extern` 常常意味着跨文件依赖，要格外注意维护成本。
- `mutable` 应谨慎使用，并确保有合理理由。
- 看到 `volatile` 时不要自动联想到“线程安全”。
