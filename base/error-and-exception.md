# 异常与错误处理

很多其它语言开发者在进入 `C++` 时，会自然问一个问题：`C++` 到底应该用返回值处理错误，还是用异常处理错误？

答案不是一句话能说完，但有一个很重要的前提：

> 在 `C++` 里，错误处理不只是控制流问题，还和资源管理、析构时机、接口设计强相关。

也就是说，如果你只把异常理解成“另一种 return”，会漏掉很多关键点。

## 先区分两类错误

可以先把错误粗略分成两类：

- 业务层面的可预期失败，例如用户名已存在、文件不存在、参数非法
- 程序无法继续按正常路径执行的异常情况，例如内存分配失败、解析过程内部不满足前置条件

真实项目里，很多团队会根据场景混合使用返回值和异常，而不是只用一种方式。

## 返回值处理错误

最直接的方式是返回状态：

```cpp
#include <iostream>
#include <string>

bool parsePort(const std::string& text, int& port) {
    if (text.empty()) {
        return false;
    }

    port = std::stoi(text);
    return true;
}

int main() {
    int port = 0;
    if (!parsePort("8080", port)) {
        std::cout << "parse failed" << std::endl;
        return 1;
    }

    std::cout << port << std::endl;
    return 0;
}
```

这种方式的优点是：

- 显式
- 控制流清楚
- 没有异常传播成本和心智负担

缺点是：

- 错误处理逻辑容易散落
- 返回值和真正业务结果可能混在一起
- 如果每一步都要检查，样板代码会很多

## 错误码

旧式 `C` / `C++` 项目里很常见的一种形式是返回错误码：

```cpp
enum class ErrorCode {
    Ok,
    InvalidInput,
    NotFound
};
```

然后函数返回 `ErrorCode`，调用方判断分支。这种方式到今天仍然很常见，尤其是在：

- 对异常禁用或慎用的代码库
- 底层系统代码
- 接口规范非常明确的模块

## 什么是异常

异常是另一种错误传递机制。最基本形式如下：

```cpp
#include <iostream>
#include <stdexcept>

int divide(int a, int b) {
    if (b == 0) {
        throw std::runtime_error("division by zero");
    }

    return a / b;
}

int main() {
    try {
        std::cout << divide(10, 2) << std::endl;
        std::cout << divide(10, 0) << std::endl;
    } catch (const std::exception& ex) {
        std::cout << "error: " << ex.what() << std::endl;
    }

    return 0;
}
```

你可以先把它理解成：

- `throw`：抛出错误
- `try`：尝试执行
- `catch`：捕获并处理错误

## 异常真正重要的地方：栈展开

`C++` 异常最核心的价值，不只是“跳到别处处理错误”，而是：

> 当异常离开当前作用域时，已经构造好的局部对象会按逆序自动析构。

这叫栈展开。

看例子：

```cpp
#include <iostream>
#include <stdexcept>

class Guard {
public:
    explicit Guard(const char* name) : name_(name) {
        std::cout << "enter " << name_ << std::endl;
    }

    ~Guard() {
        std::cout << "leave " << name_ << std::endl;
    }

private:
    const char* name_;
};

void run() {
    Guard a("A");
    Guard b("B");
    throw std::runtime_error("boom");
}
```

如果 `run()` 抛出异常，`b` 和 `a` 的析构函数仍会被调用。这就是为什么 `RAII` 和异常安全在 `C++` 里总是绑在一起讨论。

## RAII 为什么能简化错误处理

假设你手动写资源释放：

```cpp
File* file = open();
doSomething();
close(file);
```

如果 `doSomething()` 中间抛异常，`close(file)` 可能就执行不到了。

而如果你把资源封装成对象：

```cpp
FileHandle file("data.txt");
doSomething();
```

那么异常发生时，`file` 离开作用域会自动析构，资源也就自动释放。也正因此，现代 `C++` 鼓励尽量把资源管理放进对象，而不是散落在流程代码里。

## 标准异常类型

标准库里常见的异常基类是：

```cpp
std::exception
```

一些常见派生类型包括：

- `std::runtime_error`
- `std::logic_error`
- `std::invalid_argument`
- `std::out_of_range`

一般来说，如果只是写学习或中小型业务代码，先熟悉这些标准异常就足够了。

## 捕获异常时的常见写法

最常见的建议是按常量引用捕获：

```cpp
catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
}
```

原因是：

- 避免复制异常对象
- 保留多态信息
- 书写习惯统一

## 不要滥用异常

异常不是应该四处乱扔的“高级控制流”。以下情况通常不适合用异常：

- 一个完全可预期、且需要频繁分支处理的普通业务结果
- 正常循环里的大量分支控制
- 仅仅因为不想写 `if` 判断就把所有失败都改成异常

如果一个失败是日常业务的一部分，很多场景用返回值会更清晰。

## 构造函数里如何处理失败

`C++` 里构造函数不能返回错误码，因此当构造对象时就无法继续建立有效状态，抛异常往往是合理选择。

例如打开文件失败、建立连接失败、初始化关键资源失败等场景，经常会在构造阶段直接抛出异常。

这再次说明为什么对象设计和错误处理经常分不开。

## `noexcept`

`C++11` 引入了 `noexcept`，用来表达“这个函数承诺不抛异常”：

```cpp
void cleanup() noexcept {
}
```

对初学者来说，可以先记住两点：

- 它是一种接口承诺
- 在移动构造、析构和底层基础设施代码里经常出现

先不用过早沉迷其全部细节，但读代码时要知道它意味着“异常行为被显式约束”。

## 异常与析构函数

一个非常重要的经验是：

> 析构函数通常不应该抛出异常。

因为如果程序在异常传播过程中又有析构函数抛异常，会让程序进入更危险的状态，通常直接终止。

这也是为什么很多资源清理代码即便可能失败，也会选择记录错误、吞掉异常或提供单独的关闭接口，而不是在析构里继续抛。

## 一个更完整的示例

```cpp
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

std::string readFirstLine(const std::string& fileName) {
    std::ifstream input(fileName.c_str());
    if (!input) {
        throw std::runtime_error("failed to open file");
    }

    std::string line;
    if (!std::getline(input, line)) {
        throw std::runtime_error("failed to read line");
    }

    return line;
}

int main() {
    try {
        std::cout << readFirstLine("demo.txt") << std::endl;
    } catch (const std::exception& ex) {
        std::cout << "error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
```

这个例子里：

- `ifstream` 用对象管理文件资源
- 打开失败和读取失败通过异常向外传播
- 调用方在边界处统一捕获并打印错误

这就是现代 `C++` 里很常见的一种组织方式。

## 面向其它语言开发者的理解方式

如果你来自其它语言，可以这样类比：

- 和 `Go` 的显式错误返回相比，异常更像把错误交给更外层统一处理。
- 和 `JS` 的 `throw` / `try...catch` 相比，`C++` 更强调异常传播过程中的对象析构与资源释放。
- 和 `PHP` / `TS` 类似，你也能定义自己的异常类型，但在 `C++` 里资源安全更值得优先考虑。

## 常见误区

### 误区 1：异常只是另一种返回值

不对。异常会改变控制流，并触发栈展开，这会直接影响资源释放行为。

### 误区 2：既然有异常，就不需要 `if`

不对。很多业务上的普通失败本来就更适合返回值判断。

### 误区 3：用了智能指针就不用考虑异常安全

不完全对。智能指针能帮你管理部分资源，但接口设计、事务性操作和状态回滚仍然要自己思考。

### 误区 4：析构里抛异常也没关系

通常不建议这么做，风险很高。

## 推荐实践

- 在模块边界统一决定错误处理策略，而不是每个函数都各写一套。
- 资源获取尽量交给对象管理，让异常传播时仍能自动清理。
- 捕获标准异常时优先使用 `const std::exception&`。
- 对日常可预期失败，优先考虑清晰的返回值设计。
- 不要把异常当成普通分支跳转工具。

## 小结

在 `C++` 里，错误处理和资源管理是同一张图上的两个部分。你一旦理解了异常传播、栈展开和 RAII 的关系，就会明白为什么现代 `C++` 常常强调“把资源绑进对象生命周期里”。这比单纯记住 `try/catch` 语法更重要，也更接近真实工程实践。
