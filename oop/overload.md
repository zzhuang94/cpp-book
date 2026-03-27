# 重载

重载是 `C++` 表达能力强大的一个重要来源，但也是歧义和误用的高发区。到了 `C++11`，重载的讨论不能只停留在“参数不同即可重载”，还必须理解 `nullptr`、左值/右值、拷贝/移动构造以及 `=delete` 对重载设计的影响。

# 函数重载

最基本的函数重载：

```cpp
#include <iostream>

void print(int value) {
    std::cout << "int: " << value << std::endl;
}

void print(double value) {
    std::cout << "double: " << value << std::endl;
}
```

编译器会根据实参类型选择最匹配的版本。

# 重载解析

隐式类型转换可能让结果变得不直观：

```cpp
void log(int) {
}

void log(long) {
}

// log(10u); // 可能产生你未预期的匹配结果
```

这也是现代 `C++` 更强调接口清晰和类型明确的原因。

# `nullptr`

旧代码里：

```cpp
void f(int);
void f(int*);

// f(NULL); // 可能歧义
```

`C++11` 使用 `nullptr` 后：

```cpp
f(nullptr);
```

语义就明确得多。

# 引用重载

`C++11` 让你可以根据左值和右值做不同重载：

```cpp
#include <iostream>
#include <string>

void process(const std::string& text) {
    std::cout << "lvalue or const ref: " << text << std::endl;
}

void process(std::string&& text) {
    std::cout << "rvalue ref: " << text << std::endl;
}
```

这在性能敏感接口中很常见。

# 构造重载

类通常会有多个构造函数：

```cpp
#include <iostream>
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

在 `C++11` 中，委托构造让这种设计更简洁。

# 拷贝与移动

这部分是现代类设计最需要重视的重载之一：

```cpp
#include <iostream>
#include <string>
#include <utility>

class Buffer {
public:
    Buffer(const std::string& data) : data_(data) {
    }

    Buffer(const Buffer& other) : data_(other.data_) {
        std::cout << "copy ctor" << std::endl;
    }

    Buffer(Buffer&& other) : data_(std::move(other.data_)) {
        std::cout << "move ctor" << std::endl;
    }

private:
    std::string data_;
};
```

这意味着对象在“复制”和“转移资源”时可以采用不同策略。

# 运算符重载

`C++` 允许对很多运算符做自定义：

```cpp
#include <iostream>

class Point {
public:
    Point(int x, int y) : x_(x), y_(y) {
    }

    Point operator+(const Point& other) const {
        return Point(x_ + other.x_, y_ + other.y_);
    }

    void print() const {
        std::cout << "(" << x_ << ", " << y_ << ")" << std::endl;
    }

private:
    int x_;
    int y_;
};

int main() {
    Point a(1, 2);
    Point b(3, 4);
    Point c = a + b;
    c.print();
    return 0;
}
```

运算符重载的原则是：

> 只有当新语义与原运算符的直觉含义高度一致时，才值得重载。

# `=delete` 控制

`C++11` 可以通过删除函数精确禁止某些调用：

```cpp
class Number {
public:
    void set(int value) {
    }

    void set(double) = delete;
};
```

这能比注释更强硬地限制误用。

# 推荐实践

- 重载应服务于接口清晰，而不是炫技。
- 涉及左值/右值时，明确区分引用版本。
- 对不希望发生的隐式调用，可以使用 `=delete`。
- 运算符重载要保持直觉一致，避免制造“看起来像内置运算，实际语义很怪”的接口。
- 出现多个相近重载时，要特别警惕隐式转换歧义。

# 小结

`C++11` 让重载不仅是“多写几个同名函数”，而是和移动语义、接口约束、类型安全深度绑定。写得好的重载会让 API 非常自然；写得差的重载则会让调用点充满猜谜感。

## 重载在现代 C++ 里为什么更需要克制

`C++` 的重载能力很强，但越强的能力越容易被用过头。真实项目里重载最容易带来的问题不是“不会用”，而是：

- 同名函数太多，调用点难以判断走哪个版本
- 隐式转换让结果不直观
- 左值/右值版本过多，接口理解成本上升

所以重载应该服务于清晰，而不是服务于“看起来很灵活”。

## `=delete` 是现代重载控制的重要工具

以前很多不希望发生的调用只能靠注释提醒，现在可以直接在类型系统层面禁止。例如：

```cpp
void set(double) = delete;
```

这会让错误调用在编译期直接暴露，而不是留到运行时或文档说明里。

## 左值/右值重载不要只看性能

这类重载确实和性能有关，但更深层的价值是表达语义边界：

- 左值通常意味着调用方后续还会继续使用这个对象
- 右值通常意味着资源可以被转移

如果没有明确语义收益，不必为了“更现代”而到处写左值/右值双版本。

## 常见误区

### 误区 1：重载越多，API 越友好

不对。重载过多常常会让使用者更困惑。

### 误区 2：隐式转换总会帮你选到对的函数

不对。它也是歧义和误选的重要来源。

### 误区 3：能用运算符重载就尽量用

不对。只有语义高度贴合时才值得重载。

## 补充建议

- 重载数量控制在容易理解的范围内。
- 不希望发生的调用用 `=delete` 明确禁止。
- 左值/右值版本只在确有价值时提供。
- 运算符重载务必保持直觉一致。
