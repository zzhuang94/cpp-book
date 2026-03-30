# 结构体

结构体是 `C++` 中非常基础但又容易被低估的类型工具。`struct` / `class` 语法上几乎一样，最主要的默认差异是：
- `struct` 默认成员公开
- `class` 默认成员私有

> 在现代 `C++` 中，`struct` 常常是表达轻量数据模型的非常好用的手段。
----

# 基本用法

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

----

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

----

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

----

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
----

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

----

# 适用场景

通常适合：

- 简单数据聚合
- 轻量 DTO/配置对象
- 临时返回多个值
- 没有复杂封装需求的模型

当类型需要严格封装、不变量控制、复杂行为时，再考虑 `class` 更合适。

----

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

    // 这里没有写成 `Book b2{"C++11", 500};`，因为这个例子里的 `Book` 带有默认成员初始值
    // 不能使用聚合初始化
    Book b2;
    b2.title = "C++11";
    b2.pages = 500;


    b1.print();
    b2.print();
    return 0;
}
```

`struct` 这一章真正有用的，不是区分它和 `class` 的语法细节，而是学会把它当成一种意图信号：
> 这个类型主要是轻量数据载体，还是已经开始承载复杂封装规则。读代码时，这个判断会非常省力。

----

# 推荐实践

- 纯数据模型、轻量配置对象优先考虑 `struct`。
- 需要明确封装和约束时使用 `class`。
- 简单结构体优先使用聚合初始化和成员默认值。
- 即使是 `struct`，也可以合理加入成员函数和 `const` 接口。

# 小结

- 现代 `C++` 里的 `struct` 不是低配版 `class`，而是更适合轻量数据模型的一种默认表达。
- 只要把“数据载体”和“强封装对象”区分开，`struct` 和 `class` 的使用边界就会清楚很多。
