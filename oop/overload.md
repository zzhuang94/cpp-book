# 重载

重载是 `C++` 表达能力强大的重要来源，但它同时也是歧义和误用的高发区。到了 `C++11`，重载已经不能只理解成“同名函数参数不同”，还必须把 `nullptr`、左值/右值、拷贝/移动构造，以及 `=delete` 对接口边界的影响一起看。

写得好的重载会让 API 很自然；写得差的重载则会让调用点像在猜谜。

----

# 函数重载

最基本的函数重载如下：

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

## 重载成立的基本条件

函数名相同还不够，真正决定能否重载的是参数列表是否不同。  
返回类型不同本身并不能形成合法重载，例如下面这种写法是不允许的：

```cpp
int parse();
double parse(); // 非法
```

所以判断一组函数是不是重载时，先看的是：

- 参数个数是否不同
- 参数类型是否不同
- 参数顺序是否不同

## 不要把重载和默认参数混在一起想

默认参数虽然也能让“同一个函数看起来支持多种调用方式”，但它和重载不是一回事。

例如：

```cpp
void log(int level, bool newline = true);
```

这只是一个函数，只不过第二个参数可以省略；而下面才是两个重载：

```cpp
void log(int level);
void log(int level, bool newline);
```

两者在接口设计上都可能成立，但要注意：  
如果默认参数和重载混用得太多，调用点会更难判断“到底匹配了哪个版本”。

----

# 重载解析

隐式类型转换可能让匹配结果变得不直观：

```cpp
void log(int) {
}

void log(long) {
}

// log(10u); // 可能产生你未预期的匹配结果
```

这正是很多重载问题的根源：调用点看起来简单，编译器却在背后做了复杂匹配。

## 编译器大致会怎么选

可以先建立一个粗略但实用的直觉：

1. 优先选择无需转换的精确匹配
2. 其次考虑代价较小的标准转换
3. 如果多个候选同样“差不多合适”，就可能出现歧义

因此只要你看到下面这些情况，就要提高警惕：

- 多个数值类型重载并存
- 指针和整数版本并存
- 允许大量隐式转换的构造函数参与匹配

现代 `C++` 更强调接口清晰和类型明确，本质上就是为了减少这类“编译器懂了，人却看不懂”的情况。

## 一个更直观的歧义例子

```cpp
void print(long) {
}

void print(double) {
}

// print(10); // 这里对读者来说就已经不够直观
```

像这种“两个版本都能接，而且都不是特别自然”的重载集合，就很容易让调用点失去可读性。

所以设计重载时，一个很实用的标准是：

> **不要只问编译器能不能选出来，还要问人能不能一眼看懂为什么选这个。**

----

# `nullptr`

旧代码里常见这种写法：

```cpp
void f(int);
void f(int*);

// f(NULL); // 可能歧义
```

问题在于 `NULL` 往往只是一个整数零常量，既可能被当成整数，也可能被当成空指针语义。

`C++11` 使用 `nullptr` 后：

```cpp
f(nullptr);
```

语义就明确得多，因为 `nullptr` 就是专门表示空指针的字面量。

## 为什么这对重载尤其重要

重载设计里最怕“调用看起来像一回事，类型系统却把它当另一回事”。  
`nullptr` 的价值就在于把“空指针意图”直接写进类型系统里，从而减少整数版本和指针版本之间的歧义。

----

# 引用重载

`C++11` 让你可以根据左值和右值提供不同重载：

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

这在性能敏感接口中很常见，但它的意义不只在性能。

## 左值/右值重载真正表达了什么

可以把两种参数大致理解为：

- 左值：调用方后面大概率还会继续使用这个对象
- 右值：这是一个临时对象，资源有机会被转移

所以右值引用重载通常不仅在优化复制成本，也是在表达语义边界。  
不过如果没有明确收益，不必为了“更现代”而到处写双版本。

## 一眼看懂调用会走哪边

```cpp
#include <string>

void process(const std::string& text) {
}

void process(std::string&& text) {
}

int main() {
    std::string name = "alice";
    process(name);                  // 左值，通常匹配 const std::string&
    process(std::string("bob"));    // 右值，通常匹配 std::string&&
}
```

这个区分之所以有意义，不只是因为“右值能更快”，还因为它在表达：

- 这个对象后面还要继续用，别随便搬走资源
- 这个对象是临时值，可以安全地转移内部资源

----

# 成员函数重载

重载不只发生在普通函数里，成员函数也可以重载。

最常见的一类，是根据 `const` 性质区分：

```cpp
#include <string>

class Text {
public:
    char& operator[](std::size_t index) {
        return data_[index];
    }

    const char& operator[](std::size_t index) const {
        return data_[index];
    }

private:
    std::string data_ = "hello";
};
```

这里两个 `operator[]` 参数列表看起来一样，但一个是普通成员函数，一个是 `const` 成员函数，因此可以形成重载。

## 这种重载在表达什么

它表达的不是“两种完全不同的操作”，而是：

- 可修改对象，返回可修改引用
- 只读对象，返回只读引用

这类重载非常符合直觉，因此通常是好的重载设计。

----

# 构造重载

类通常会有多个构造函数：

```cpp
#include <string>

class User {
public:
    User() : User("guest") {
    }

    explicit User(const std::string& name) : name_(name) {
    }

private:
    std::string name_;
};
```

在 `C++11` 中，委托构造让这种设计更简洁。

## 构造重载最容易踩的坑

构造函数也是重载体系的一部分，所以它同样可能受到隐式转换影响。  
尤其是单参数构造函数，如果不加 `explicit`，就可能在你没预料到的地方参与类型转换和重载决议。

因此，单参数构造函数如果不是明确要支持隐式转换，通常更建议写成：

```cpp
explicit User(const std::string& name);
```

这样可以显著降低接口误用概率。

----

# 拷贝与移动

这部分是现代类设计里最需要重视的重载之一：

```cpp
#include <iostream>
#include <string>
#include <utility>

class Buffer {
public:
    explicit Buffer(const std::string& data) : data_(data) {
    }

    Buffer(const Buffer& other) : data_(other.data_) {
        std::cout << "copy ctor" << std::endl;
    }

    Buffer(Buffer&& other) noexcept : data_(std::move(other.data_)) {
        std::cout << "move ctor" << std::endl;
    }

private:
    std::string data_;
};
```

这意味着对象在“复制”和“转移资源”时可以采用不同策略。

## 为什么这类重载很重要

在早期理解里，构造函数重载更多像“提供几种初始化方式”。  
到了 `C++11`，拷贝构造和移动构造的重载还承担了对象语义表达的职责：

- 拷贝表示产生一个独立副本
- 移动表示把可转移资源接收过来

因此这类重载已经不是单纯的语法技巧，而是类设计的一部分。

----

# 重载、重写、隐藏

这是 `C++` 初学者最容易混淆的三个词。

## 先一句话区分

- 重载：同一作用域里，同名函数参数不同
- 重写：派生类重新实现基类虚函数
- 隐藏：派生类定义了同名函数，把基类同名函数遮住了

看一个对比例子：

```cpp
class Base {
public:
    void print(int) {
    }

    virtual void run() {
    }
};

class Derived : public Base {
public:
    void print(double) {
    } // 这不是重写，而是隐藏了 Base::print

    void run() override {
    } // 这才是重写
};
```

这里最容易误会的是 `print(double)`。  
很多人看到同名函数就以为“这是新增一个重载”，但在继承场景里，派生类里的同名声明会先把基类同名函数隐藏掉。

所以在 `Derived` 对象上直接调用：

```cpp
Derived d;
// d.print(1); // 看到的是 Derived::print(double)
```

此时重载集合并不会自动把 `Base::print(int)` 和 `Derived::print(double)` 合并在一起给你选。

## 如果想把基类重载重新带进来

可以显式写：

```cpp
class Derived : public Base {
public:
    using Base::print;

    void print(double) {
    }
};
```

这时基类里的 `print(int)` 才会重新进入当前作用域的候选集合。

这也是为什么“重载”和“继承”一旦放在一起，就特别容易让人困惑。

----

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

运算符重载最重要的原则是：

> 只有当新语义与原运算符的直觉含义高度一致时，才值得重载。

## 什么时候不该重载运算符

如果一个操作本身没有明显的数学或逻辑对应关系，就不要为了“写起来酷”而硬套运算符。  
否则读代码的人会看到熟悉的符号，却得到完全陌生的语义。

典型风险包括：

- `+` 并不表示组合或求和
- `==` 不是在表达等价关系
- `<<` 被拿去做和流输出毫无关系的业务动作

这类设计虽然语法合法，但通常会显著降低代码可读性。

----

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

## 为什么它是现代重载控制的重要工具

以前很多“不希望发生的调用”只能靠文档或注释提醒，现在可以直接在类型系统层面阻止。  
这会带来两个直接好处：

- 错误调用在编译期暴露
- 接口意图明确，不需要调用方猜测

所以 `=delete` 不只是“禁用某个函数”，它还是设计重载集合时的重要约束工具。

## 它还可以用来挡掉错误类型

例如你只想接受布尔值，不想让整数偷偷参与调用：

```cpp
class Switch {
public:
    void set(bool enabled) {
    }

    void set(int) = delete;
};
```

这样 `set(true)` 是允许的，但 `set(1)` 会在编译期直接报错。

这类做法的价值在于：

- 调用者不会误以为“差不多的类型也能传”
- 接口边界被写成了可检查的语言规则
- 重载集合更接近你的真实设计意图

----

# 推荐实践

- 重载应服务于接口清晰，而不是为了展示语言技巧。
- 出现多个相近重载时，要特别警惕隐式转换歧义。
- 不要让默认参数和大量重载一起堆叠，避免调用规则变得难以推断。
- 单参数构造函数如果不希望参与隐式转换，优先加 `explicit`。
- 涉及左值/右值时，只在确有语义或性能收益时区分引用版本。
- 在继承体系里，要分清重载、重写和同名隐藏不是一回事。
- 对不希望发生的调用，可以使用 `=delete`。
- 运算符重载要保持直觉一致，避免制造“看起来像内置运算，实际语义很怪”的接口。

----

# 小结

重载在现代 `C++` 里早就不只是“同名函数多写几个版本”这么简单。它和类型转换、值类别、对象语义、接口约束，甚至继承中的名字查找规则都紧密相关。

真正好的重载设计通常有两个特征：调用点读起来直观，编译器的选择也符合人的预期。只要你开始用这个标准检查 API，就能避开大部分重载滥用问题。
