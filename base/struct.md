# 结构体

`struct` 是 `C++` 中非常基础但又容易被低估的类型工具。很多人把它只理解成“公开成员的类”，这并不算错，但在现代 `C++` 中，`struct` 常常是表达轻量数据模型的非常好用的手段。

## 最基本的结构体

```cpp
#include <iostream>
#include <string>

struct User {
    int id;
    std::string name;
};

int main() {
    User user{1, "alice"};
    std::cout << user.id << ", " << user.name << std::endl;
    return 0;
}
```

## `struct` 与 `class` 的区别

语法上几乎一样，最主要的默认差异是：

- `struct` 默认成员公开
- `class` 默认成员私有

也就是说，这两段代码在能力上并没有本质鸿沟。

## 聚合初始化

`C++11` 中，很多简单结构体可以直接用花括号初始化：

```cpp
struct Point {
    int x;
    int y;
};

Point p{10, 20};
```

这种写法非常直观，适合轻量数据载体。

## 成员默认值

`C++11` 允许类和结构体成员在定义处提供默认值：

```cpp
#include <string>

struct Config {
    int port = 8080;
    bool debug = false;
    std::string host = "127.0.0.1";
};
```

这能显著减少样板构造代码。

## 在 `struct` 中定义成员函数

`struct` 不只是被动数据容器，也可以拥有行为：

```cpp
#include <iostream>

struct Point {
    int x;
    int y;

    void print() const {
        std::cout << "(" << x << ", " << y << ")" << std::endl;
    }
};
```

## 构造函数

```cpp
#include <iostream>
#include <string>

struct User {
    int id;
    std::string name;

    User(int userId, const std::string& userName)
        : id(userId), name(userName) {
    }
};
```

这说明 `struct` 完全可以参与面向对象设计，只是默认访问权限不同。

## `struct` 适合什么场景

通常适合：

- 简单数据聚合
- 轻量 DTO/配置对象
- 临时返回多个值
- 没有复杂封装需求的模型

当类型需要严格封装、不变量控制、复杂行为时，再考虑 `class` 更合适。

## 一个综合示例

```cpp
#include <iostream>
#include <string>

struct Book {
    std::string title = "unknown";
    int pages = 0;

    void print() const {
        std::cout << title << ", " << pages << std::endl;
    }
};

int main() {
    Book b1;
    Book b2{"C++11", 500};

    b1.print();
    b2.print();
    return 0;
}
```

## 推荐实践

- 纯数据模型、轻量配置对象优先考虑 `struct`。
- 需要明确封装和约束时使用 `class`。
- 简单结构体优先使用聚合初始化和成员默认值。
- 即使是 `struct`，也可以合理加入成员函数和 `const` 接口。

## 小结

现代 `C++` 中的 `struct` 不应被当成“比 `class` 低一级”的东西。它更像是在告诉读者：这个类型主要是数据载体，成员默认公开，使用方式偏轻量。配合 `C++11` 的初始化能力，`struct` 会变得非常顺手。
