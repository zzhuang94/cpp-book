# 函数重载

> 重载是 `C++` 非常有代表性的语言能力之一。

很多语言也支持“同名函数接收不同参数”，但在 `C++` 里，重载和下面这些内容都紧密相连：

- 类型系统
- 隐式类型转换
- 左值 / 右值
- `const` 成员函数
- 构造函数设计
- 运算符语义
- 模板与普通函数的匹配关系

所以它绝不是“一个函数写多个版本”这么简单。

如果说封装、继承、多态体现了 `C++` 面向对象的一面，那么重载体现的就是 `C++` 在“接口表达力”上的一面。  
设计得好，调用点会非常自然；设计得差，调用点就会变成“编译器能看懂，但人看不懂”。

----

# 为什么需要重载

先看一个最直观的例子：

```cpp
#include <iostream>
#include <string>

void print(int value) {
    std::cout << "int: " << value << std::endl;
}

void print(double value) {
    std::cout << "double: " << value << std::endl;
}

void print(const std::string& value) {
    std::cout << "string: " << value << std::endl;
}
```

这里三个函数名字都叫 `print`，但处理的对象不同。  
这样设计的好处是：
- 对调用者来说，操作意图一致，都是“打印”
- 不需要为了参数类型不同而硬拆成 `printInt()`、`printDouble()`、`printString()`
- 接口名字表达的是“行为”，不是“实现细节”

> 也就是说，重载的核心价值，是让同一种语义动作在不同参数形式下保持统一接口

这一点非常重要，因为它直接决定了一个重载是不是“值得存在”：
- 如果几个函数真的在做同一种事，重载通常是自然的
- 如果几个函数只是“凑巧想共用一个名字”，那重载往往会让接口更混乱

----

# 函数重载

```cpp
int add(int a, int b) {
    return a + b;
}

double add(double a, double b) {
    return a + b;
}
```

这就是最典型的函数重载。编译器会根据调用时的实参类型，选择最匹配的版本。

```cpp
add(1, 2);        // 调用 int 版本
add(1.5, 2.5);    // 调用 double 版本
```

## 重载的标准

在同一作用域中，函数名相同，且参数列表不同，才构成合法重载。  
这里的“参数列表不同”，常见包括：

- 参数个数不同
- 参数类型不同
- 参数顺序不同

例如：

```cpp
void log(int level);
void log(int level, const std::string& msg);
void log(const std::string& msg, int level);
```

这三个都可以共存。

### 返回类型不同，不能单独形成重载

这是初学者最常见的误区之一。

```cpp
int parse();
double parse(); // 非法
```

调用一个函数时，编译器首先要根据 `函数名 + 参数` 找到候选函数，而不是先看你准备把返回值接到什么类型里。  
如果仅靠返回类型区分，那么像 `parse();` 这种不接收返回值的调用就根本无法判断到底该选哪一个。

所以要牢牢记住： **返回类型可以不同，但不能只靠返回类型不同来重载。**

### 这些情况也不能形成重载

有些写法看起来“好像不同”，其实在函数签名层面并不构成新的重载。

#### 仅参数名不同

```cpp
void f(int x);
void f(int y); // 不是重载，重复声明
```

参数名只是函数体内部使用的标识符，不参与重载区分。

#### 顶层 `const` 修饰值传递参数

```cpp
void f(int value);
void f(const int value); // 不是重载
```

因为按值传参时，调用者传进来的本来就是一份副本。  
对这份副本加不加顶层 `const`，只影响函数体内部能不能改它，不影响调用方式。

#### 数组参数与指针参数

```cpp
void f(int arr[]);
void f(int* arr); // 不是重载
```

因为函数参数中的数组类型会退化成指针类型。

#### 仅 typedef 或 using 换个名字

```cpp
using MyInt = int;

void f(int);
void f(MyInt); // 不是重载
```

别名不是新类型，本质还是同一个类型。

## 默认参数

> 默认参数可以让一个函数支持多种调用方式，但它和重载不是一回事。

```cpp
// 这里还是一个函数，只是第二个参数可以省略。
void connect(const std::string& host, int port = 80);

// 而下面才是真正的重载：
void connect(const std::string& host);
void connect(const std::string& host, int port);
```

两种设计都可能合理，但混在一起时就容易出问题。例如：

```cpp
void log(int level);
void log(int level, bool newline = true);

// log(1); // 歧义
```

调用 `log(1)` 时：

- 第一个版本直接匹配
- 第二个版本也能匹配，因为第二个参数有默认值

于是编译器就会报歧义。

所以一个很实用的经验是：

- 如果一个接口只是“少传几个参数也能成立”，优先考虑默认参数
- **如果不同调用形式背后对应不同语义分支，才考虑重载**
- 不要让“默认参数”和“多个相近重载”一起堆得太多

----

# 编译器如何选择重载

> 重载真正难的地方，不在“能不能写出来”，而在“编译器到底会选哪个”。

编译器在候选函数中通常会优先选择：
1. 不需要转换的精确匹配
2. 代价较小的标准转换
3. 代价更高的用户自定义转换
4. 如果多个候选都差不多合适，就报歧义

你不需要一开始就把标准条文背下来，但要建立一个稳定直觉：**越少转换、越自然、越直接的匹配，优先级通常越高。**

## 精确匹配

```cpp
void show(int) {
}

void show(double) {
}

show(10);    // 精确匹配 int
show(3.14);  // 精确匹配 double
```

这种情况通常最好理解，也最值得追求。

## 标准转换

如果没有精确匹配，编译器可能会做标准转换。

```cpp
void show(long) {
}
void show(double) {
}
show(10);
// 编译失败：error: call of overloaded 'show(int)' is ambiguous
```

这里 `10` 是 `int` 字面量。它既能转成 `long`，也能转成 `double`。  
这时对人来说，调用就已经不那么直观了。即使某些编译器最终能给出结果，这样的接口也不够友好。  
这类情况提醒我们：
- 不要只看“编译器能不能选出来”
- 还要看“读代码的人能不能一眼判断为什么选这个”

*像这种设计，虽然每个单独的重载都合法，但它们放在一起后，调用点可读性明显下降。*

所以设计重载集合时，一个很好的标准是：
> - **调用点是否具备“可预测性”。**
> - 如果调用者需要停下来想“这次到底会选哪个版本”，那这组重载就已经不够理想了。

----

# 隐式转换与重载陷阱

## 隐式转换

所谓隐式转换，就是： **你没有手动写转换代码，但编译器为了让一段代码成立，自动帮你做了类型转换**。比如：

```cpp
double x = 10;
```

这里右边的 `10` 本来是 `int`，但赋值给 `double` 时，编译器会自动把它转成 `double`。  
你没有写 `static_cast<double>(10)`，但转换确实发生了，这就是隐式转换。

再比如：

```cpp
void show(double value) {
}

show(3);
```

`show` 需要的是 `double`，但你传进去的是 `int`；编译器会先把 `3` 转成 `double`，然后再完成调用。

很多时候，这种机制很方便，因为它让调用写起来更自然。  
但问题也恰恰在这里：**一旦重载参与进来，隐式转换就不只是“能不能调用”的问题，而是“到底该选哪个函数”的问题。**

重载和隐式转换一旦碰到一起，很多“看上去没问题”的接口就会开始变危险。

## 数值类型混用

```cpp
void set(int) {
}

void set(double) {
}
```

像 `set(10)`、`set(3.14)` 这种调用还比较清楚，但下面这些就不一定了：

```cpp
short s = 1;
float x = 2.5f;
char c = 'A';

set(s);
set(x);
set(c);
```

这些实参很多都要经过提升或转换后才能匹配。  
接口设计一旦同时出现多个“相近数值类型版本”，调用行为就会越来越不直观。

所以在工程里，常见建议是：

- 如果确实只有一种自然类型，就只提供这一种
- 如果多个数值类型都支持，要确保调用规则对人足够明显
- 不要为了“接口看起来灵活”而堆很多相近重载

## 单参数构造函数

看下面这个类：

```cpp
#include <string>

class Name {
public:
    Name(const char* text) : value_(text) {
    }

private:
    std::string value_;
};
```

如果你再写：

```cpp
void print(Name name) {
}
```

那么 `print("alice")` 也是可能成立的，因为编译器可以先把 `"alice"` 转成 `Name`。  
这就是为什么单参数构造函数会影响重载解析。

> 如果这种隐式转换不是你明确想要的，通常应在构造函数前加上 `explicit` 修饰符：

```cpp
class Name {
public:
    explicit Name(const char* text) : value_(text) {
    }

private:
    std::string value_;
};
```

这样可以显著减少“意外参与重载”的情况。

## =delete 主动封堵

有时候你不是“想支持很多版本”，而是“只想支持一种，顺便把容易误用的版本封掉”。

```cpp
class Switch {
public:
    void set(bool enabled) {
    }
    void set(int) = delete;
};
```

这样：

```cpp
set(true); // 合法
set(1);    // 编译报错
```

这比写文档提醒“请不要传 `1`”更有效。从接口设计角度看，`=delete` 的价值在于：
> - 把“不允许的调用”写进类型系统
> - 把错误暴露在编译期
> - 让重载集合更接近你的真实意图

----

# nullptr 和空指针重载

> 这是现代 `C++` 中非常重要的一类场景。

旧代码里常见：

```cpp
void f(int);
void f(int*);

// f(NULL); // 可能有问题
```

`NULL` 在很多实现里本质只是整数零常量，因此它既可能去匹配整数版本，也可能表达“空指针”的意思。  
这就让重载变得不稳定。

`C++11` 引入 `nullptr` 之后：

```cpp
void f(int);
void f(int*);

f(nullptr); // 明确表达空指针语义
```

`nullptr` 有专门的空指针类型语义，不再是“顺手拿整数零兼职扮演空指针”。  
所以一旦接口里同时出现：

- 整数版本
- 指针版本

就应该优先使用 `nullptr`，而不是 `NULL` 或 `0`。  
这不仅是语法升级，更是为了让重载解析更清晰。

----

# 引用重载

引用重载是现代 `C++` 里非常值得重点理解的一类。

## const T& 和 T&&

```cpp
#include <iostream>
#include <string>
#include <utility>

void process(const std::string& text) {
    std::cout << "read: " << text << std::endl;
}
void process(std::string&& text) {
    // 此处没有真实发生移动，只是将 text 的资源转移给了形参
    std::cout << "move-ready: " << text << std::endl;
}

int main() {
    std::string name = "alice";

    process(name);               // 左值，通常匹配 const std::string&
    process(std::string("bob")); // 右值，通常匹配 std::string&&
    process("hello");            // 可能构造临时 string，再匹配右值版本
    process(std::move(name));    // 把 name 转成右值，触发移动构造

    std::cout << "name = " << name << std::endl;
    return 0;
}
```

这组重载的意义不只是性能，更是在表达对象语义：

- 左值通常表示“调用方后面可能还要继续使用它”
- 右值通常表示“这是临时值，可以考虑转移资源”

### T&、const T&、T&& 同时出现

```cpp
void use(std::string& s) {}
void use(const std::string& s) {}
void use(std::string&& s) {}
```

可以粗略理解为：

> - `T&`：只接收可修改左值
> - `const T&`：*能接收只读左值，也能接收右值*
> - `T&&`：**优先** 接收右值

这套体系很强，但也容易让接口变复杂。  
如果三种版本的行为差异并不明显，就不一定值得全都写出来。

### 什么时候值得区分左值 / 右值

通常是下面几类场景：

- 确实希望针对临时对象做移动优化
- 左值和右值在语义上需要不同处理
- 类型内部资源昂贵，复制代价明显

> 如果只是普通读取接口，`const T&` 往往已经足够自然。

----

# 成员函数

> 成员函数也可以形成重载，而且这是类设计里很常见的一类。

## 根据参数不同重载

```cpp
class Printer {
public:
    void print(int value) {
    }
    void print(double value) {
    }
};
```

这和普通函数重载没有本质区别。

## 根据 const 重载

这类非常重要。

```cpp
#include <iostream>
#include <string>

class Text {
public:
    char& operator[](std::size_t index) {
        return data_[index];
    }

    const char& operator[](std::size_t index) const {
        return data_[index];
    }

    void print() const {
        std::cout << "text = " << data_ << std::endl;
    }

private:
    std::string data_ = "hello";
};

int main() {
    Text text;
    text[0] = 'H';
    std::cout << "text[0] = " << text[0] << std::endl;
    text.print();

    const Text constText;
    // constText[1] = 'W';
    // 编译错误： error: assignment of read-only location 'constText.Text::operator[](1)'
    std::cout << "constText[1] = " << constText[1] << '\n';
    constText.print();

    return 0;
}
```

> 示例代码是运算符重载

这里两个 `operator[]` 的参数列表看起来一样，但依然可以重载，因为：

- 一个是普通成员函数
- 一个是 `const` 成员函数

它们背后的隐含区别是 `this` 指针类型不同。可以把它理解为：

- 非 `const` 对象调用时，返回可修改引用
- `const` 对象调用时，返回只读引用

这类重载很常见，也非常符合直觉，因此通常是“好的重载设计”。

## 引用限定成员函数

现代 `C++` 里，成员函数还可以根据对象本身是左值还是右值继续细分：

```cpp
#include <iostream>
#include <string>
#include <utility>

class Buffer {
public:
    Buffer(std::string text) : data_(std::move(text)) {}

    std::string& data() & {
        std::cout << "调用左值版本 data() &" << std::endl;
        return data_;
    }

    std::string&& data() && {
        std::cout << "调用右值版本 data() &&" << std::endl;
        return std::move(data_);
    }

    void print() const {
        std::cout << "data = " << data_ << std::endl;
    }

private:
    std::string data_;
};

int main() {
    Buffer buf("hello");

    std::string a = buf.data();
    std::cout << "a = " << a << std::endl;
    a = "hhh";
    std::cout << "a = " << a << std::endl;
    buf.print();

    std::string& aa = buf.data();
    std::cout << "aa = " << aa << std::endl;
    aa = "aaa";
    std::cout << "aa = " << aa << std::endl;
    buf.print();

    std::string b = Buffer("world").data();
    std::cout << "b = " << b << std::endl;

    return 0;
}
```

这里的 `&` 和 `&&` 不是返回引用，而是**成员函数的引用限定符**。

它表达的是：

- 对左值对象调用 `data()`，返回左值引用
- 对临时对象调用 `data()`，可以安全地把内部数据移动出来

**注意对别 a 和 aa 的区别**
- `std::string a = buf.data();` a 是新对象，修改 a 不影响原始 buf
- `std::string& aa = buf.data();` aa 是引用，直接绑定 buf.data_

这种设计比普通重载更进阶，但它说明了一件事：

> 重载不只发生在“参数不同”上，它还可以围绕对象状态和调用语境来表达接口语义。

----

# 构造函数

> 构造函数本身就是重载体系的重要部分。

```cpp
#include <string>

class User {
public:
    User() : name_("guest"), age_(0) {}
    explicit User(const std::string& name) : name_(name), age_(0) {}
    User(const std::string& name, int age) : name_(name), age_(age) {}
private:
    std::string name_;
    int age_;
};
```

这组构造重载表达的是不同初始化方式。

> 构造函数天然就可能参与隐式转换，构造重载需要特别谨慎。

例如不加 `explicit` 时：

```cpp
class User {
public:
    User(const std::string& name) : name_(name) {}
private:
    std::string name_;
};

void greet(User user) {
}

// greet("alice"); // 可能发生隐式构造
```

**所以单参数构造函数如果不是明确想支持“自动转换”，通常应优先写成 `explicit`。**

## 委托构造

在 `C++11` 里，构造重载经常配合委托构造一起使用：

```cpp
class User {
public:
    User() : User("guest", 0) {}
    explicit User(const std::string& name) : User(name, 0) {}
    User(const std::string& name, int age) : name_(name), age_(age) {}
private:
    std::string name_;
    int age_;
};
```

这样多个构造版本共享一套核心初始化逻辑，更容易保持一致。

----

## 拷贝和移动

> 很多人初学时把拷贝构造、移动构造看成“特殊规则”，但从语言视角看，它们本质上也是重载。

具体参考 [《类和对象》](oop/base.md) `拷贝和移动` 一节

所以一旦进入现代 `C++` 类设计，重载已经不是“语法技巧”，而是对象语义设计的一部分。

----

# 重载、重写、隐藏

- 重载：同一作用域里，同名函数参数不同
- 重写：`派生类` 重新实现 `基类` 虚函数
- 隐藏：派生类声明了同名函数，把基类同名函数遮住

```cpp
class Base {
public:
    void print(int) {}
    virtual void run() {}
};

class Derived : public Base {
public:
    void print(double) {}
    void run() override {}
};
```

这里：

- `run()` 是重写
- `print(double)` 不是对 `Base::print(int)` 的重写
- *它会把基类里同名的 `print` 隐藏掉*

> 在继承场景里，很多人以为“基类一个版本 + 派生类一个版本 = 一组重载”，其实未必成立。

如果想把基类版本重新带进来:

```cpp
class Derived : public Base {
public:
    using Base::print;

    void print(double) {
    }
};
```

`using Base::print;` 的作用是把基类同名函数重新引入当前作用域，形成完整候选集合。

这也是为什么： **重载不仅和参数列表有关，还和名字查找规则有关。**

----

# 推荐实践

- 只有当多个函数版本确实表达的是 **同一种操作语义** 时，再使用重载。
- 优先让不同重载之间只在“参数形式”上变化，而不是在“行为含义”上变化。
- 能不依赖隐式转换就尽量不依赖，避免让重载决议变得模糊。
- 单参数构造函数如果不是明确想提供自动转换，优先加上 `explicit`。
- 在左值 / 右值、`const` / 非 `const` 重载中，要保证接口语义自然且稳定。
- 继承场景里注意名字隐藏问题，必要时使用 `using Base::func;` 把基类重载重新引入。
- 当重载版本越来越多、规则越来越绕时，要及时考虑模板、默认参数或重构接口，而不是继续堆叠同名函数。

----

# 小结

- 重载的核心价值，不是“一个名字写很多次”，而是让 **同一种语义动作** 在不同参数形式下保持统一接口。
- 它背后牵涉到的不只是参数个数和类型，还包括名字查找、隐式转换、`const` 属性、引用类别、构造函数语义以及继承规则。
- 设计得好的重载会让调用点自然、稳定、可读；设计得差的重载则容易制造歧义、隐藏和误用。

所以学习重载，真正要掌握的不只是“语法能不能写”，而是： **这个接口是否表达清楚了设计意图，编译器和人是否都能稳定地理解它。**