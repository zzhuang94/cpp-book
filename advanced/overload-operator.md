# 运算符重载

运算符重载是 `C++` 最有特色、也最容易被滥用的能力之一。  
运算符重载的本质不是“发明新运算符”，而是：
> **让已有运算符在自定义类型上拥有合理、自然、可预测的语义。**

**`C++` 大多数常见运算符都可以重载**，例如：

- 算术运算符：`+`、`-`、`*`、`/`、`%`
- 比较运算符：`==`、`!=`、`<`、`>`
- 赋值相关：`=`、`+=`、`-=`
- 下标与调用：`[]`、`()`
- 递增递减：`++`、`--`
- 指针语义：`*`、`->`
- 流运算符：`<<`、`>>`

但也有一些不能重载，例如：

- `.` 成员访问运算符
- `.*`
- `::`
- `?:`
- `sizeof`

此外，即使某个运算符能重载，也有两条规则不会变：

- 不能改变运算符优先级
- 不能改变运算符结合性

> 也就是说，重载改变的是“对某个类型执行时的语义”，不是整门语言的语法规则。

----

# 写法对比

> 运算符重载既可以写成成员函数，也可以写成非成员函数。

## 成员函数 

```cpp
#include <iostream>
#include <string>

class Point {
public:
    Point(int x, int y) : x_(x), y_(y) {}

    Point operator+(const Point& other) const {
        return Point(x_ + other.x_, y_ + other.y_);
    }

    void print(const std::string& label) const {
        std::cout << label << " = (" << x_ << ", " << y_ << ")" << std::endl;
    }

private:
    int x_;
    int y_;
};

int main() {
    Point a(1, 2);
    Point b(3, 4);
    Point sum = a + b;

    a.print("点 a");
    b.print("点 b");
    sum.print("a + b 的结果");

    return 0;
}
```

这里 `a + b` 表示两个点坐标分别相加，语义很自然，因此这种重载通常是合理的。

## 非成员运算符

```cpp
#include <iostream>

class Point {
public:
    Point(int x, int y) : x_(x), y_(y) {}
    int x() const { return x_; }
    int y() const { return y_; }
private:
    int x_;
    int y_;
};
Point operator+(const Point& a, const Point& b) {
    return Point(a.x() + b.x(), a.y() + b.y());
}
std::ostream& operator<<(std::ostream& os, const Point& point) {
    os << "(" << point.x() << ", " << point.y() << ")";
    return os;
}
int main() {
    Point left(5, 8);
    Point right(2, 6);
    Point result = left + right;
    std::cout << "left  = " << left << std::endl;
    std::cout << "right = " << right << std::endl;
    std::cout << "left + right = " << result << std::endl;
    return 0;
}
```

## 如何选择

一个实用直觉是：
> - 如果运算天然需要修改左操作数本身，成员函数往往更自然
> - 如果希望左右两边保持对称，非成员函数常常更合适

比如 `+=` 通常更适合写成成员函数，因为它要修改左操作数。  
而 `+` 常常可以基于 `+=` 写成非成员函数，以获得更自然的对称性。

```cpp
#include <iostream>

class Point {
public:
    Point(int x, int y) : x_(x), y_(y) {}
    int x() const { return x_; }
    int y() const { return y_; }
    Point& operator+=(const Point& other) {
        x_ += other.x_;
        y_ += other.y_;
        return *this;
    }
private:
    int x_;
    int y_;
};
Point operator+(Point lhs, const Point& rhs) {
    lhs += rhs;
    return lhs;
}
std::ostream& operator<<(std::ostream& os, const Point& point) {
    os << "(" << point.x() << ", " << point.y() << ")";
    return os;
}

int main() {
    Point a(1, 1);
    Point b(2, 3);
    std::cout << "初始时 a = " << a << ", b = " << b << std::endl;
    a += b;
    std::cout << "执行 a += b 后，a = " << a << std::endl;
    Point c = a + b;
    std::cout << "再执行 c = a + b 后，c = " << c << std::endl;
    return 0;
}
```

这种写法很常见，因为：

- `+=` 负责原地修改
- `+` 复用 `+=` 的逻辑
- 接口含义清晰，代码也不重复

----

# 常见运算符重载

## 赋值运算符 operator=

赋值通常返回 `*this` 的引用：

```cpp
#include <iostream>

class Number {
public:
    explicit Number(int value = 0) : value_(value) {}

    Number& operator=(const Number& other) {
        if (this != &other) {
            value_ = other.value_;
        }
        return *this;
    }

    void print(const char* label) const {
        std::cout << label << " = " << value_ << std::endl;
    }

private:
    int value_;
};

int main() {
    Number a(1);
    Number b(2);
    Number c(3);

    a = b = c;

    std::cout << "执行链式赋值 a = b = c 之后：" << std::endl;
    a.print("a");
    b.print("b");
    c.print("c");

    return 0;
}
```

这样才能支持链式赋值，例如 `a = b = c;`。

## 下标运算符 operator[]

最常见的是成对提供 `const` 与非 `const` 版本：

```cpp
#include <cstddef>
#include <iostream>
#include <vector>

class IntArray {
public:
    IntArray() : data_{1, 2, 3} {}
    int& operator[](std::size_t index) { return data_[index]; }
    const int& operator[](std::size_t index) const { return data_[index]; }

    void print(const char* label) const {
        std::cout << label << ": ";
        for (std::size_t i = 0; i < data_.size(); ++i) {
            std::cout << data_[i];
            if (i + 1 != data_.size()) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }

private:
    std::vector<int> data_;
};

int main() {
    IntArray numbers;
    numbers[1] = 42;

    const IntArray readonly_numbers = numbers;

    numbers.print("可修改对象 numbers");
    std::cout << "readonly_numbers[1] = " << readonly_numbers[1] << std::endl;

    return 0;
}
```

这样：

- 非 `const` 对象可以修改元素
- `const` 对象只能读取元素

## 函数调用运算符 operator()

> 重载 `operator()` 的对象叫函数对象。

```cpp
#include <iostream>

class Adder {
public:
    explicit Adder(int base) : base_(base) {}
    int operator()(int x) const { return base_ + x; }
private:
    int base_;
};
int main() {
    Adder add10(10);
    int result = add10(5);
    std::cout << "add10(5) 的结果是 " << result << std::endl;
    return 0;
}
```

使用时可以写成 `Adder add10(10); int result = add10(5);`。  
看起来像调用函数，其实是在调用对象。

## 前置 ++ 和后置 ++

> 这两个版本是经典面试点，也是非常典型的 `同一运算符的重载区分方式`。

```cpp
#include <iostream>

class Counter {
public:
    explicit Counter(int value = 0) : value_(value) {}
    int value() const { eturn value_; }

    Counter& operator++() {
        ++value_;
        return *this;
    }
    Counter operator++(int) {
        Counter old = *this;
        ++value_;
        return old;
    }
private:
    int value_;
};

int main() {
    Counter counter(0);
    std::cout << "初始值: " << counter.value() << std::endl;

    Counter old_post = counter++;
    std::cout << "执行 counter++ 后，返回旧值 " << old_post.value()
              << "，当前对象变为 " << counter.value() << std::endl;

    Counter& now_pre = ++counter;
    std::cout << "执行 ++counter 后，返回当前值 " << now_pre.value()
              << "，当前对象也为 " << counter.value() << std::endl;

    return 0;
}
```

> - `operator++()` 表示前置 `++x`
> - `operator++(int)` 表示后置 `x++`

这里那个 `int` 参数只是语法标记，不表示真的会传入有实际意义的整数。通常：
- 前置版本返回引用
- 后置版本返回旧值副本


## 流插入运算符 operator<<

这是最常见的非成员运算符重载之一。

```cpp
#include <iostream>
#include <string>
#include <utility>

class User {
public:
    User(std::string name, int age) : name_(std::move(name)), age_(age) {}
    const std::string& name() const { return name_; }
    int age() const { return age_; }
private:
    std::string name_;
    int age_;
};
std::ostream& operator<<(std::ostream& os, const User& user) {
    os << "User{name=" << user.name() << ", age=" << user.age() << "}";
    return os;
}
int main() {
    User user("Alice", 20);
    std::cout << "输出用户对象: " << user << std::endl;
    return 0;
}
```

为什么通常写成非成员函数？

因为左操作数是 `std::ostream`，不是你的类对象。  
如果写成成员函数，就变成要求你去改 `std::ostream`，这显然不现实。

所以像 `<<`、`>>` 这种“左边是流对象”的场景，通常写成非成员函数最自然。

----

# 设计原则

- **语义必须直观:**  如果一个运算符的行为和内置类型经验差得太远，读者就会被误导。
  - `+` 应该像“求和 / 合并 / 拼接”
  - `==` 应该表达“相等或等价”
  - `[]` 应该像“按索引访问”
  - `()` 应该像“调用一个动作”
- **尽量保持代数直觉**： 例如如果你重载了：`+/+=/==` 那最好保证它们之间的关系符合读者预期。
  - `a += b` 的效果应与 `a = a + b` 在语义上大体一致
  - `a == b` 应该稳定、无副作用
- **优先保证对称性与可读性**
  - 如果一个二元运算本来就具有对称意味，常常要注意左右操作数能否自然工作。
  - 例如一个坐标类如果既想支持 `point + point`，也想支持 `2 + point`，
  - 那么成员函数写法未必够灵活，非成员函数可能更自然。
- **不要制造副作用惊喜**
  - 像 `+` 这种运算符，读者通常默认它不会改左操作数。  
  - 像 `+=`，读者通常默认它会改左操作数。
  - 这套直觉非常宝贵，不要破坏它。
- **`const` 要合理**
  - 例如把 `operator+` 写成 `Point operator+(const Point& other) const;`
  - 这里末尾的 `const` 很重要，因为“相加”通常不应修改当前对象。
  - 同样，如果重载比较运算、输出运算，也常常应尽量保持只读语义。

----

# 限制规则

运算符重载虽然强大，但它不是无限自由的。

- **不能改变参与者个数**
  - 你不能把一元运算符硬改成三元，也不能把二元运算符改成别的形状。
- **不能发明新符号**
  - 你只能重载语言已有的运算符，不能自己创造类似 `**`、`<+>` 这样的新运算符。
- **至少要有一个操作数是自定义类型**
  - 你不能为两个内置类型重新定义运算规则。例如不能去改 `int + int` 的意义。
- **优先级和结合性固定**
  - 例如即使你重载了 `operator*`，它依然保持乘法运算符原有的优先级和结合性，不会变成你自定义的新语法。

----

# 推荐实践

- 只有当一个运算符的行为与读者直觉基本一致时，才去重载它。
- 优先让 `+`、`-`、`==`、`[]`、`()`
  这些运算符继续表达它们原本常见的语义，不要“借壳表达别的意思”。
- `operator+` 这类通常应保持无副作用，而 `operator+=` 这类修改型运算符才负责改变对象本身。
- 如果某个二元运算需要强调对称性，优先考虑使用非成员函数实现，这样左右操作数更自然。
- 像 `<<`、`>>` 这类左操作数不是当前类对象的运算符，通常应写成非成员函数。
- 比较运算、输出运算以及不会修改对象状态的运算，尽量保持 `const` 语义。
- 不要为了“语法看起来酷”而重载运算符；如果普通成员函数写法更清晰，就应优先使用普通函数接口。
- **一旦重载之后需要频繁向别人解释“这里其实不是那个意思”，通常就说明这个设计不够好。**

----

# 小结

- 运算符重载的核心价值，是让自定义类型在使用体验上更接近内置类型，同时保持清晰语义。
- 它本质上还是函数，只是把函数调用写成了运算符形式，因此同样要遵守接口设计的基本原则。
- 成员函数和非成员函数的选择，往往取决于左操作数是谁、是否需要对称性，以及是否要访问对象内部状态。
- **真正值得学习的重点，不是“某个运算符怎么写出来”，而是“这个运算符重载后，别人能不能不费力地读懂它”。**

所以运算符重载写得好，会让类像语言内建的一部分；写得不好，则会让代码表面简洁、实际难懂。

