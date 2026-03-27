# 修饰符

- 这里的“修饰符”主要指那些会改变类型、对象生命周期、链接属性或语义边界的关键字与限定符。
- 对于重新上手 `C++` 的开发者来说，这部分最容易“看得懂代码，但说不清为什么这么写”。

# signed / unsigned

```cpp
#include <iostream>

int main() {
    signed int a = -5;
    unsigned int b = 5;

    std::cout << a << ", " << b << std::endl;
    return 0;
}
```

- 在工程代码里，`unsigned` 往往和大小、索引、位字段绑定
- 但它也会制造很多隐式转换问题，因此不能因为“不会小于 0”就机械使用。

# short / long / long long

这些修饰符主要影响取值范围。`C++11` 时代，`long long` 已经是很常用的大整数类型。

```cpp
long long id = 9223372036854775807LL;
```

# const

`const` 修饰“不能修改”，是最常用的语义修饰符之一。

```cpp
const int maxRetry = 3;
```

它还会影响函数参数、成员函数、指针、引用等语义，后续章节会详细展开。

# volatile

`volatile` 表示对象的值可能在程序控制之外发生变化，例如硬件寄存器或某些极特殊并发场景。

```cpp
volatile int* status = nullptr;
```

> `volatile` 不是线程同步工具。在现代并发程序里，线程间可见性和原子性应优先使用 `std::atomic`、互斥锁等机制。

# static

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

# extern

`extern` 用于声明“这个变量或函数定义在别处”：

```cpp
extern int globalValue;
```

这在多文件项目里非常常见，尤其是头文件声明、源文件定义的配合。

# mutable

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

- 修饰符这一章最值得练的，不是把关键字逐个记住，而是读声明时主动问一句：“这个修饰符额外表达了什么语义？” 
- 一旦形成这个习惯，`static`、`extern`、`mutable` 这些看似分散的知识点就会更容易串起来。

# 推荐实践

- `const` 尽量多用，让接口语义更明确。
- 对整数范围有要求时再选择 `short`、`long`、`long long`，不要靠习惯。
- 谨慎使用 `unsigned`，特别是和有符号整型混合比较时。
- 不要把 `volatile` 当成并发同步手段。
- 需要表达类级别共享状态时使用 `static` 成员。

# 小结

- 修饰符的价值，在于把额外语义直接写进声明本身。
- 只要你开始带着“它改变了什么边界”去读这些关键字，很多原本零散的规则就会变得更容易理解，也更不容易误用。
