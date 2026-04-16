# 所有权与 RAII

如果说很多语言把注意力放在“对象能做什么”，那么现代 `C++11` 还会反复追问另一件事：

> 这个对象拥有谁？谁负责释放资源？对象离开作用域时会发生什么？

这就是为什么很多人明明看得懂 `if/for/class`，一进入真实 `C++` 项目却还是不敢改代码。  
难点常常不在控制流，而在 **资源和生命周期**。

这篇文章的目标，就是把所有权和 `RAII` 这条主线讲清楚，默认你已经知道构造函数、析构函数、拷贝和移动这些语法名字。

----

# 所有权意义

在很多带垃圾回收的语言里，你平时不需要频繁思考“谁负责释放资源”。  
但在 `C++` 里，这不是边缘问题，而是日常接口设计的一部分。

你会经常遇到这些判断：

- 这个对象是按值保存，还是通过指针间接引用？
- 这个函数只是借用参数，还是接管它？
- 这个资源离开作用域时，会不会自动释放？
- 这段代码异常退出时，会不会漏清理？

如果这些问题没有被明确写出来，项目就很容易出现：

- 内存泄漏
- 重复释放
- 使用已失效对象
- 生命周期判断错误
- 代码表面能跑，但维护成本很高

----

# 三种关系

读 `C++` 代码时，可以先把对象关系粗略分成三类：

- **拥有**： 某个对象负责资源的生命周期，资源何时释放由它决定。
- **借用**： 某个对象或函数只是临时使用资源，但不负责释放。
- **观察**： 某个对象只保留一个“能找到它”的引用或指针，用来访问，但不拥有。

这三种关系在语法上不总是 100% 一一对应，但这个分类对理解工程代码非常有帮助。  
也可以把它看成对前面 `类和对象` 一章的继续：那一章更偏对象语义，这一章更偏资源语义。

----

# RAII 模型

`RAII` 是 `Resource Acquisition Is Initialization` 的缩写，通常翻成：**资源获取即初始化**

它的核心思想非常简单：

- 在对象构造时获取资源
- 在对象析构时释放资源

这意味着资源管理不再散落在业务流程里，而是绑定到对象生命周期上。

例如：

```cpp
#include <cstdio>

class FileHandle {
public:
    explicit FileHandle(const char* path)
        : file_(std::fopen(path, "w")) {
    }

    ~FileHandle() {
        if (file_ != nullptr) {
            std::fclose(file_);
        }
    }

private:
    std::FILE* file_ = nullptr;
};
```

这个类的意义不是“把 `FILE*` 包起来”这么简单，而是：

- 资源的获取和释放被绑定到了对象生命周期上
- 调用者不需要在流程里手动记住 `fclose`
- 即使中途 `return` 或抛异常，析构仍然会执行

----

# RAII 优势

看一个对比。

```cpp
std::FILE* file = std::fopen("demo.txt", "w");
if (file == nullptr) {
    return;
}

do_work();
std::fclose(file);
```

表面上看没问题，但一旦中间多了：

- 提前 `return`
- 新分支
- 异常
- 更多资源

就很容易出现清理遗漏。

而如果改成：

```cpp
FileHandle file("demo.txt");
do_work();
```

清理时机就被语言作用域天然托底了。

这也是为什么现代 `C++` 一直强调：**优先让对象自己管理资源，而不是在业务流程里散落清理逻辑。**

----

# 资源类型

很多初学者会把“资源”狭义地理解成内存，但工程里资源远不止这个。常见资源包括：

- 动态内存
- 文件句柄
- socket
- 互斥锁
- 数据库连接
- 第三方库句柄
- 临时目录或系统对象

只要某个东西需要“成对地获取和释放”，它通常就适合用 RAII 思维去管理。

----

# 所有权形式

## 栈对象

很多时候最稳的默认选择就是直接按值创建对象：

```cpp
Config config;
Logger logger;
```

这类对象的生命周期由作用域决定，离开作用域时自动析构。

对很多场景来说，最好的资源管理不是“更复杂的智能指针”，而是：**如果能不用堆对象，就先不用。**

## 按值成员

```cpp
class User {
private:
    std::string name_;
    std::vector<int> scores_;
};
```

这通常表示：

- `User` 拥有这些成员
- 成员生命周期跟随 `User`
- 不需要手动释放

## 指针成员

```cpp
class Service {
private:
    Logger* logger_;
};
```

这里就不能只靠语法猜测语义了。  
它可能表示：

- `Service` 拥有 `logger_`
- `Service` 只是借用 `logger_`
- `logger_` 可能为空

这也是为什么现代 `C++11` 更提倡用智能指针或更清晰的接口形式来表达所有权。

----

# 裸指针语义

这是接手现代 `C++` 项目时最该尽快建立的一条直觉：

> 裸指针默认更适合表示“观察”或“可为空借用”，而不是“负责释放”。

例如：

```cpp
void print(const User* user);
```

这通常表示：

- 函数只是读取 `user`
- 函数不拥有它
- 参数允许为空

如果你看到裸指针就自动脑补“最后谁来 `delete`”，很多现代代码会被你误读。

----

# 引用语义

```cpp
void normalize(std::vector<int>& values);
void print(const std::string& text);
```

这两类接口通常表达的是：

- `T&`：借用并允许修改
- `const T&`：只读借用

它们通常不表示所有权转移。

所以现代 `C++` 里经常会形成一套很重要的默认判断：

- 值对象：拥有
- 引用：借用
- 裸指针：可为空借用或观察
- 智能指针：显式所有权

----

# 异常与 RAII

只要代码里可能抛异常，你就不能只想“正常路径最后会不会释放”，还要想：

> 如果中途跳出当前作用域，资源还能不能稳定回收？

看这个例子：

```cpp
void run() {
    FileHandle file("data.txt");
    std::vector<int> values{1, 2, 3};

    process(values); // 这里可能抛异常
}
```

如果 `process()` 抛异常：

- `values` 会析构
- `file` 会析构
- 资源仍然能自动回收

这正是 RAII 真正的工程价值。

----

# 常见误区

很多人刚学到 RAII 时，会把它理解成：

> “写个析构函数去 delete 掉指针”

这只说对了一小部分。

RAII 更重要的意思是：

- 让对象承担资源管理职责
- 让作用域负责触发清理
- 让异常路径和正常路径都能共享同一套释放规则

所以现代 `C++11` 最理想的状态往往不是“我手写了很多析构函数”，而是：

> 我优先使用本身就具备 RAII 语义的类型，例如 `string`、`vector`、`unique_ptr`、锁包装器等。

----

# 资源类设计

如果你直接面对某种需要显式释放的外部资源，往往就应该考虑自己做一个 RAII 封装。

典型场景包括：

- 第三方 C 库返回的句柄
- 需要 `open/close` 的对象
- 需要 `lock/unlock` 的过程
- 需要成对初始化和销毁的系统资源

这时的设计重点通常是：

1. 构造时是否获取资源
2. 析构时如何释放资源
3. 是否允许复制
4. 是否允许移动

尤其是资源独占型对象，通常应该：

- 禁止复制
- 允许移动

----

# 独占资源类

```cpp
#include <utility>

void close_socket(int) {}  // 演示占位；工程里换成真实关闭逻辑

class SocketHandle {
public:
    explicit SocketHandle(int fd) : fd_(fd) {}
    ~SocketHandle() {
        if (fd_ != -1) {
            close_socket(fd_);
        }
    }

    SocketHandle(const SocketHandle&) = delete;

    // 删掉的是拷贝赋值运算符，只在出现赋值表达式、且左操作数是 SocketHandle 对象时才会用到
    SocketHandle& operator=(const SocketHandle&) = delete;

    SocketHandle(SocketHandle&& other) noexcept : fd_(other.fd_) {
        other.fd_ = -1;
    }

    SocketHandle& operator=(SocketHandle&& other) noexcept {
        if (this != &other) {
            if (fd_ != -1) {
                close_socket(fd_);
            }
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }

private:
    int fd_ = -1;
};

void test(SocketHandle handle) {
    // do something
}

void testRef(SocketHandle& handle) {
    // do something
}

int main() {
    SocketHandle a(3);
    SocketHandle b = std::move(a);  // 转移给 b，a 内部 fd 被置为 -1

    // SocketHandle c = b;          // 错误：拷贝构造已 =delete，无法编译

    // 这是声明一个引用并把它绑定到已有对象 b上，语法上看起来像 =，
    // 但语义是引用初始化（绑定），不是对对象的赋值。
    SocketHandle &e = b;


    // test(e); // 这里会调用拷贝构造函数，因为 e 是引用，不是对象，所以会调用拷贝构造函数。
    testRef(e); // 这里可以正常使用

    SocketHandle d(7);
    d = std::move(b);               // 移动赋值：先释放 d 原资源，再接管 b 的资源

    return 0;
}

```

这个类表达得很清楚：

- 一个句柄只能被一个对象拥有
- 不允许复制
- 可以把拥有权移动给别的对象

这就是“所有权”在类设计里的具体落地。

----

# 阅读线索

拿到陌生代码时，可以优先看下面几个信号：

1. 对象是按值保存，还是通过指针间接保存？
2. 类型里有没有析构函数？
3. 有没有 `=delete` 拷贝构造和拷贝赋值？
4. 有没有移动构造、移动赋值？
5. 参数是值、引用、裸指针还是智能指针？
6. 某个成员是不是 `unique_ptr` 或 `shared_ptr`？

这些信息经常比函数体细节更早告诉你这段代码的资源语义。

----

# 推荐实践

- 优先使用作用域和对象生命周期管理资源。
- 能按值持有就按值持有，能不用堆对象就先不用。
- 裸指针默认更多表示借用或观察，而不是拥有。
- 需要显式所有权时，优先使用智能指针表达。
- 资源类要同时考虑析构、复制和移动是否自洽。
- 遇到异常路径时，优先检查资源是否仍能通过析构稳定回收。

----

# 小结

所有权和 `RAII` 是现代 `C++11` 最核心的主线之一。

你真正需要建立的直觉不是“析构函数怎么写”，而是：

- 资源是谁拥有的
- 生命周期跟着谁走
- 异常或提前返回时还能不能稳定清理

只要这条主线立住了，后面再学智能指针、类设计、异常安全和并发时，很多看起来零散的规则都会连成一张图。
