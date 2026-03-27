# 类/对象

如果说 `C++98` 的类设计重点还是“封装数据和行为”，那么到了 `C++11`，类设计还必须表达资源所有权、拷贝/移动语义、默认行为边界和接口意图。也就是说，类不再只是“会写构造函数和析构函数”就够了。

## 最基本的类

```cpp
#include <iostream>
#include <string>

class User {
public:
    User(const std::string& name) : name_(name) {
    }

    void print() const {
        std::cout << name_ << std::endl;
    }

private:
    std::string name_;
};

int main() {
    User user("alice");
    user.print();
    return 0;
}
```

## 访问控制

类中最常见的三个访问级别：

- `public`
- `protected`
- `private`

`class` 默认是 `private`，`struct` 默认是 `public`。

## 构造函数与初始化列表

成员对象通常应通过初始化列表初始化，而不是先默认构造再赋值：

```cpp
class Point {
public:
    Point(int x, int y) : x_(x), y_(y) {
    }

private:
    int x_;
    int y_;
};
```

这在性能和语义上都更合理。

## 成员默认值

`C++11` 允许在类定义处给成员提供默认值：

```cpp
#include <string>

class Config {
private:
    int port_ = 8080;
    std::string host_ = "127.0.0.1";
};
```

这能显著减少样板构造代码。

## 委托构造

`C++11` 支持构造函数之间互相调用：

```cpp
#include <string>

class User {
public:
    User() : User("guest") {
    }

    User(const std::string& name) : name_(name) {
    }

private:
    std::string name_;
};
```

这比重复写初始化逻辑更清晰。

## `=default`

`C++11` 可以显式要求编译器生成默认实现：

```cpp
class Item {
public:
    Item() = default;
};
```

这比“什么都不写，依赖隐式生成”更清楚地表达意图。

## `=delete`

你还可以显式禁止某些操作：

```cpp
class Logger {
public:
    Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};
```

这在资源对象、单例辅助类、不可复制句柄等场景非常有用。

## 拷贝与移动

这是 `C++11` 类设计的核心升级。一个类对象可能具备：

- 拷贝构造
- 拷贝赋值
- 移动构造
- 移动赋值
- 析构函数

如果类内部管理资源，就必须认真思考这些函数的行为。

```cpp
#include <iostream>
#include <string>
#include <utility>

class Buffer {
public:
    Buffer(std::string data) : data_(std::move(data)) {
    }

    Buffer(Buffer&& other) : data_(std::move(other.data_)) {
        std::cout << "move ctor" << std::endl;
    }

private:
    std::string data_;
};
```

## RAII

现代 `C++` 类设计的一个核心思想是 RAII：资源在对象构造时获取，在析构时释放。

这意味着类的职责通常包括：

- 管理文件句柄
- 管理锁
- 管理动态内存
- 管理连接或句柄

## 一个综合示例

```cpp
#include <iostream>
#include <string>

class User {
public:
    User() = default;

    User(int id, const std::string& name)
        : id_(id), name_(name) {
    }

    void print() const {
        std::cout << id_ << ", " << name_ << std::endl;
    }

private:
    int id_ = 0;
    std::string name_ = "guest";
};

int main() {
    User u1;
    User u2(1, "alice");

    u1.print();
    u2.print();
    return 0;
}
```

## 推荐实践

- 成员优先在初始化列表中初始化。
- 简单默认行为用 `=default` 显式表达。
- 不允许的操作用 `=delete` 显式禁止。
- 有资源管理职责的类必须认真设计拷贝和移动语义。
- 优先用对象生命周期管理资源，而不是在业务流程里散落释放代码。

## 小结

`C++11` 让类设计从“封装”升级成了“封装 + 生命周期 + 资源语义 + 接口边界”。如果你要接手现代 `C++` 项目，这一套思维方式比单纯记住类语法重要得多。
