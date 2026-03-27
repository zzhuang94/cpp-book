# 结构体

`struct` 是 `C++` 中非常基础但又容易被低估的类型工具。很多人把它只理解成“公开成员的类”，这并不算错，但在现代 `C++` 中，`struct` 常常是表达轻量数据模型的非常好用的手段。

# 基本结构体

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

# `struct` 与 `class`

语法上几乎一样，最主要的默认差异是：

- `struct` 默认成员公开
- `class` 默认成员私有

也就是说，这两段代码在能力上并没有本质鸿沟。

# 聚合初始化

`C++11` 中，很多简单结构体可以直接用花括号初始化：

```cpp
struct Point {
    int x;
    int y;
};

Point p{10, 20};
```

这种写法非常直观，适合轻量数据载体。

# 成员默认值

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

# 成员函数

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

# 构造函数

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

# 适用场景

通常适合：

- 简单数据聚合
- 轻量 DTO/配置对象
- 临时返回多个值
- 没有复杂封装需求的模型

当类型需要严格封装、不变量控制、复杂行为时，再考虑 `class` 更合适。

# 综合示例

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

# 推荐实践

- 纯数据模型、轻量配置对象优先考虑 `struct`。
- 需要明确封装和约束时使用 `class`。
- 简单结构体优先使用聚合初始化和成员默认值。
- 即使是 `struct`，也可以合理加入成员函数和 `const` 接口。

# 小结

现代 `C++` 中的 `struct` 不应被当成“比 `class` 低一级”的东西。它更像是在告诉读者：这个类型主要是数据载体，成员默认公开，使用方式偏轻量。配合 `C++11` 的初始化能力，`struct` 会变得非常顺手。

## 为什么 `struct` 值得单独认真学

很多人会下意识觉得：

- `struct` 很基础
- `class` 才是更正式、更高级的东西

但在现代 `C++` 代码里，`struct` 往往恰恰是表达轻量数据模型的好工具。它能帮助你快速区分两类类型：

- 主要承载数据、边界简单的类型
- 封装复杂约束和行为的类型

这是一种很有价值的阅读信号。

## `struct` 不等于“没有行为”

现代 `C++` 中，`struct` 完全可以拥有：

- 构造函数
- 成员函数
- 默认值
- 常量接口

所以更准确的说法不是“`struct` 不能做什么”，而是“默认公开更适合轻量数据载体”。

## 聚合初始化为什么很适合初学者

例如：

```cpp
struct Point {
    int x;
    int y;
};

Point p{10, 20};
```

这类写法非常直观，尤其适合：

- 坐标
- 配置项
- 返回多个字段的小对象
- 临时数据载体

它能让对象的结构和初始化方式都更一目了然。

## 什么时候该升级成 `class`

如果一个类型逐渐出现下面这些需求，就更适合考虑 `class`：

- 需要隐藏内部状态
- 需要维护强不变量
- 需要限制随意写成员
- 构造和修改过程很复杂

也就是说，`struct` 和 `class` 不是等级关系，而更像“默认表达意图不同”的两种入口。

## 常见误区

### 误区 1：`struct` 只是 C 时代遗留

不对。现代 `C++` 中它仍然非常常用。

### 误区 2：有成员函数就不该用 `struct`

不对。轻量数据类型依然可以拥有必要行为。

### 误区 3：`class` 总比 `struct` 更专业

不对。选择应围绕封装需求和语义意图，而不是形式感。

## 补充建议

- 轻量数据模型优先考虑 `struct`。
- 需要强封装和复杂约束时再考虑 `class`。
- 能用成员默认值和聚合初始化时尽量用简单写法。
- 把 `struct` 当成表达“这个类型主要是数据”的信号。
