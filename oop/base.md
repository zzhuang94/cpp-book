# 类和对象

如果说早期 `C++` 里的类更多被当成 *把数据和函数装在一起* 的工具；  
那么到了 `C++11`，类设计已经明显升级成了另一件事：
> 它不仅要封装行为，还要表达对象的生命周期、资源所有权、可复制性、可移动性，以及接口边界。

所以学习类时，不能只停留在“会写成员函数、会写构造函数”。  
更重要的是建立一个现代直觉：**类往往同时也是资源语义和对象语义的载体。**

----

# 类

先看一个最简单的类：

```cpp
#include <iostream>
#include <string>

class User {
public:
    // explicit 关键字，禁止隐式类型转换
    explicit User(const std::string& name) : name_(name) {
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

这里已经体现了类最基本的三件事：

- 用成员变量保存状态
- 用成员函数暴露行为
- 用访问控制决定外界能看到什么

`User` 的 `name_` 被放在 `private` 区域，说明外部代码不能随意改动内部状态，只能通过类提供的接口使用对象。这正是封装的基本含义。

----

# 访问控制

类中最常见的三个访问级别是：

- `public`
- `protected`
- `private`

它们可以简单理解为：

- `public`：类对外承诺提供的接口
- `protected`：留给派生类使用的内部能力
- `private`：只属于当前类自己的实现细节

> `class` 默认访问级别是 `private`，`struct` 默认访问级别是 `public`。

访问控制不只是“限制谁能访问”，它还在表达接口边界。

例如，一个成员如果放在 `public`，等于告诉使用者：“这是类型设计的一部分，你可以依赖它。”  
相反，如果一个实现细节被放进 `private`，就意味着以后修改内部实现时，不必同时修改所有调用方。

这也是为什么很多现代代码更强调：

> - 把稳定能力放在 `public`
> - 把实现细节留在 `private`
> - 只有在继承层次确实需要复用时，才谨慎使用 `protected`

----

# 初始化列表

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

> 这不只是“写法更短”，而是更符合对象真正的构造过程。

当构造函数开始执行前，成员对象其实就已经进入构造阶段了。  
如果你不在初始化列表里给它初值，很多成员会先走一遍默认构造，然后再在函数体里被赋值一次。

初始化列表的价值主要有两点：
- 避免“先构造、后赋值”的额外步骤
- 某些成员根本必须在初始化列表里初始化

例如下面这些情况通常只能通过初始化列表完成：
- `const` 成员
- 引用成员
- 没有默认构造函数的成员对象

```cpp
class Config {
public:
    Config(int port, std::string& name) : port_(port), name_(name) {
    }

private:
    const int port_;
    std::string& name_;
};
```

> 可以把初始化列表理解成“对象诞生时就把成员放到正确状态”，而不是“先生出来，再慢慢修正”。

----

# 成员默认值

`C++11` 允许在类定义处直接给成员提供默认值：

```cpp
#include <string>

class Config {
private:
    int port_ = 8080;
    std::string host_ = "127.0.0.1";
};
```

这会让类的默认状态更集中、更清楚。

如果没有成员默认值，很多构造函数都要重复写相同的初始化逻辑。  
一旦构造函数数量变多，这类重复代码很容易让默认状态分散在多个位置。

成员默认值的好处是：

- 默认状态写在成员定义旁边，读起来直观
- 多个构造函数可以共享默认行为
- 只有少数特殊构造函数才需要覆盖默认值

例如：

```cpp
#include <iostream>
#include <string>

class User {
public:
    User() = default;
    User(int id, const std::string &name) : id_(id), name_(name) { 
    }

    void print() const {
        std::cout << "id: " << id_ << ", name: " << name_ << std::endl;
    }

private:
    int id_ = 0;
    std::string name_ = "guest";
};

int main() {
    User user1;
    User user2(1, "alice");
    user1.print();
    user2.print();
    return 0;
}

```

读这个类时，你不必到每个构造函数里找“默认 id 是多少”“默认 name 是什么”，因为这些信息已经放在最该出现的位置了。

----

# 委托构造

`C++11` 支持构造函数之间互相调用，这叫委托构造：

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

一个类经常会有多个构造入口，例如：

- 无参构造
- 只传一个关键参数
- 传完整参数集

如果每个构造函数都自己写一遍初始化逻辑，就容易出现：

- 某些成员漏初始化
- 多个构造函数行为不一致
- 后期修改时要改很多地方

委托构造的核心价值就是把“真正的初始化逻辑”集中到一个主构造函数里，让其它构造函数只是转发参数。

> 这和普通函数里“提取公共逻辑”的思路很像，只不过这里提取的是对象初始化逻辑。

----

# =default

`C++11` 可以显式要求编译器为某个特殊成员函数生成默认实现。

```cpp
class User {
public:
    User() = default;
};
```

这里的意思不是“这个函数什么都不做”，而是：**请编译器按默认规则生成这个函数。**  
这比“完全不写”更清楚，因为它把设计意图写到了代码里。  

很多初学者看到 `=default` 时，会误以为它只是“可有可无的简写”。  
其实它最重要的价值是：**把原本隐式的行为，变成显式的接口声明。**

常见场景有三个：

- 明确表示“这个类允许默认构造”
- 自己写了别的构造函数后，重新把默认构造函数要回来
- 明确表示“拷贝/赋值就按成员默认规则处理”

```cpp
#include <iostream>
#include <string>

class User {
public:
    User() = default; // 明确允许默认构造

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
    User u1;              // 可以，走默认构造
    User u2(1, "alice");  // 也可以，走自定义构造
    u1.print();
    u2.print();
}
```

如果上面不写 `User() = default;`，而只保留带参数构造函数，那么 `User u1;` 就会直接编译失败。

> 所以 `=default` 的本质不是“偷懒”，而是： **这个函数我没有自定义逻辑，但我明确希望它存在。**

----

# =delete

`=delete` 和 `=default` 正好相反。

它不是“请编译器生成”，而是： **这个函数在语法上我承认它是一个候选操作，但我明确禁止你调用。**

最典型的场景，是某个类型不应该被复制：

```cpp
class Logger {
public:
    Logger() = default;               // 允许默认构造
    Logger(const Logger&) = delete;   // 禁止拷贝构造
    Logger& operator=(const Logger&) = delete; // 禁止拷贝赋值
};
```

这在资源对象、句柄封装类、互斥锁辅助对象等场景非常常见。

下面这个例子更容易一眼看出差异：

```cpp
class Session {
public:
    Session() = default;                    // 允许：无参创建对象
    Session(const Session&) = delete;       // 禁止：复制对象
    Session& operator=(const Session&) = delete;
};

int main() {
    Session s1;         // 正常，使用默认构造
    Session s2(s1);     // 编译错误：拷贝构造被删除
}
```

可以把它们理解成两个明确的设计动作：

- `=default`：这个操作要保留，而且按编译器默认规则实现
- `=delete`：这个操作不要给用户用，谁调用谁报错

> 所以两者的差异不是“一个能自动实现，一个不能自动实现”，而是： **一个在表达“允许”，另一个在表达“禁止”。**

如果一个类型不应该被复制，只在注释里写“请勿复制”是不够的。  
真正可靠的做法，是让错误用法在编译期直接失败。

----

# 析构函数

前面已经多次提到“对象离开作用域会自动清理资源”，这里就要正式介绍析构函数。  
析构函数的语法很固定：它的名字是类名前面加 `~`，没有参数，也没有返回值。

```cpp
class User {
public:
    ~User() {
    }
};
```
## 调用时机

通常有三种常见时机：

- 栈对象离开作用域时
- `delete` 一个动态创建的对象时
- 临时对象生命周期结束时

```cpp
#include <iostream>

class Trace {
public:
    Trace() {
        std::cout << "construct" << std::endl;
    }

    ~Trace() {
        std::cout << "destruct" << std::endl;
    }
};

int main() {
    {
        Trace t;
    } // 离开作用域，这里自动调用析构函数
}

// 运行时会先输出 `construct`，再输出 `destruct`。
```

## 现代C++是否需要析构函数

需要，但更准确地说是：**不是不需要析构函数了，而是很多时候不需要你手写析构函数了。**  
原因在于现代 `C++` 更鼓励把资源交给已经会自己清理的成员对象管理，例如：

- `std::string`
- `std::vector`
- `std::unique_ptr`
- 各种标准库容器和智能指针

例如下面这个类通常完全不用手写析构函数：

```cpp
#include <string>
#include <vector>

class User {
private:
    std::string name_;
    std::vector<int> scores_;
};
```

虽然你没有写 `~User()`，但析构函数依然存在。  
只是编译器帮你生成了默认析构函数，而成员对象在析构时会继续自动清理自己的资源。

这也是现代 `C++` 常说的**规则零**：
> 如果成员自己就能正确管理资源，那你的类最好什么特殊成员函数都别手写。

## 手写析构函数

当类自己直接管理某种 `需要显式释放` 的资源时，就往往需要手写析构函数。  
例如封装文件句柄：

```cpp
#include <cstdio>

class FileWriter {
public:
    explicit FileWriter(const char* path) {
        file_ = std::fopen(path, "w");
    }

    ~FileWriter() {
        if (file_ != nullptr) {
            std::fclose(file_);
        }
    }

private:
    std::FILE* file_ = nullptr;
};
```

这里析构函数的职责非常明确：对象死亡时关闭文件。  
所以问题不是“`C++11` 还要不要析构函数”，而是：
- 析构函数始终存在
- 简单类通常可用编译器默认析构
- 资源类则必须认真设计析构行为

还有一个非常重要的连锁反应： **一旦你手写了析构函数，往往就要继续思考 `拷贝和移动` 还能不能保持正确。**

----

# 拷贝和移动

这是 `C++11` 类设计里最重要的升级之一。  
> 学习本节之前一定要先学 [《引用》](advanced/ref.md) 作为基础知识。

很多初学者一看到“拷贝构造、移动构造、拷贝赋值、移动赋值”就头大，根本原因通常不是语法太难，而是没有先想清楚：
> 这个对象到底“拥有”什么，它被复制或转移时应该发生什么？

## 一句话理解

- 拷贝：做出一个新的、独立的副本
- 移动：把原对象持有的资源转交给新对象

如果把对象想成一个“资源盒子”：
- 拷贝 = 再做一个盒子，并把内容复制一份
- 移动 = 把原来盒子里的东西直接搬走

## 调用层面的区别

```cpp
#include <iostream>
#include <string>
#include <utility>

class Buffer {
public:
    // 传值接收参数，再把临时或实参中的字符串转移到成员里
    explicit Buffer(std::string name) : name_(std::move(name)) {
    }

    // 拷贝构造：从 other 复制出一份独立内容
    Buffer(const Buffer& other) : name_(other.name_) {
        std::cout << "拷贝构造: 复制一份 \"" << other.name_ << "\" 给新对象\n";
    }

    // 移动构造：接收一个右值引用，表示 other 可以被“搬资源”
    // noexcept 很重要：很多标准容器在扩容时，只有确认移动不会抛异常，才更愿意使用移动而不是拷贝
    // : name_(std::move(other.name_)) 表示把 other.name_ 转成右值，再交给 name_ 进行移动构造
    Buffer(Buffer&& other) noexcept : name_(std::move(other.name_)) {
        // 此时当前对象已经接管了 other 原先持有的字符串内容
        std::cout << "移动构造: 把 \"" << name_ << "\" 搬给新对象\n";
        // 被移动后的对象仍然必须保持“可析构、可赋值”的有效状态
        // 这里手动改成一个可观察的值，便于演示“资源已被搬走”
        other.name_ = "[已搬空]";
    }

    void print(const std::string& tag) const {
        std::cout << tag << ": " << name_ << '\n';
    }

private:
    std::string name_;
};

int main() {
    Buffer b1("原始缓冲区");
    b1.print("b1 初始状态");

    Buffer b2 = b1;            // 复制一份，b1 还在
    Buffer b3 = std::move(b1); // 把 b1 转成右值，触发移动构造；std::move 自己并不搬东西
    // 此处注意，如果没有移动构造函数 Buffer(Buffer&& other) noexcept
    // b3 依然能够创建成功，执行了拷贝构造，b1 不会被搬空
    // 因为 Buffer(const Buffer& other) 既能接受左值(b1) 也能接受右值(std::move(b1))

    std::cout << "---- 构造完成后 ----\n";
    b1.print("b1");
    b2.print("b2");
    b3.print("b3");
}
```

可能输出：

```text
b1 初始状态: 原始缓冲区
拷贝构造: 复制一份 "原始缓冲区" 给新对象
移动构造: 把 "原始缓冲区" 搬给新对象
---- 构造完成后 ----
b1: [已搬空]
b2: 原始缓冲区
b3: 原始缓冲区
```

这里只需要先抓住一个结论：

- `b2 = b1` 表示“我想复制一份”
- `std::move(b1)` 表示“我允许把 b1 当成可被搬走资源的对象”

注意：`std::move` 本身并不移动资源，它只是把对象转换成“可以触发移动语义”的形式。  
真正怎么移动，要看你的移动构造/移动赋值如何实现。

## 资源类必须区分拷贝和移动

如果类里只是 `int`、`std::string`、`std::vector` 这类成员，很多时候默认行为已经够用。  
但如果类自己直接管理原始资源，例如一块动态内存，那事情就不能糊弄过去了。

下面这个例子虽然比前面长一点，但它把“析构、拷贝、移动”三件事完整串起来了：

```cpp
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

class Buffer {
public:
    Buffer(std::string name, std::size_t size)
        : name_(std::move(name)), size_(size), data_(new int[size]) {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = static_cast<int>(i + 1);
        }

        std::cout << "构造: " << name_
                  << " 持有地址 " << static_cast<void*>(data_) << '\n';
    }

    ~Buffer() {
        std::cout << "析构: " << name_
                  << " 释放地址 " << static_cast<void*>(data_) << '\n';
        delete[] data_;
    }

    Buffer(const Buffer& other)
        : name_(other.name_ + "_copy"),
          size_(other.size_),
          data_(other.size_ == 0 ? nullptr : new int[other.size_]) {
        if (size_ > 0) {
            std::copy(other.data_, other.data_ + size_, data_);
        }
        std::cout << "拷贝构造: 从 " << other.name_
                  << " 深拷贝出 " << name_
                  << "，新地址是 " << static_cast<void*>(data_) << '\n';
    }

    Buffer& operator=(const Buffer& other) {
        if (this == &other) {
            return *this;
        }

        int* new_data = other.size_ == 0 ? nullptr : new int[other.size_];
        if (other.size_ > 0) {
            std::copy(other.data_, other.data_ + other.size_, new_data);
        }

        delete[] data_;
        data_ = new_data;
        size_ = other.size_;
        name_ = other.name_ + "_copy_assign";
        std::cout << "拷贝赋值: 现在 " << name_
                  << " 拥有独立地址 " << static_cast<void*>(data_) << '\n';
        return *this;
    }

    Buffer(Buffer&& other) noexcept
        : name_(std::move(other.name_)),
          size_(other.size_),
          data_(other.data_) {
        std::cout << "移动构造: " << name_
                  << " 接管地址 " << static_cast<void*>(data_) << '\n';
        other.name_ = "[已搬空]";
        other.size_ = 0;
        other.data_ = nullptr;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        delete[] data_;
        name_ = std::move(other.name_);
        data_ = other.data_;
        size_ = other.size_;
        std::cout << "移动赋值: " << name_
                  << " 接管地址 " << static_cast<void*>(data_) << '\n';

        other.name_ = "[已搬空]";
        other.data_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    void set(std::size_t index, int value) {
        if (data_ != nullptr && index < size_) {
            data_[index] = value;
        }
    }

    const int* raw_data() const {
        return data_;
    }

    void print() const {
        std::cout << name_ << " | data=" << static_cast<const void*>(data_) << " | values=";
        if (data_ == nullptr) {
            std::cout << "<null>\n";
            return;
        }

        for (std::size_t i = 0; i < size_; ++i) {
            std::cout << data_[i] << ' ';
        }
        std::cout << '\n';
    }

private:
    std::string name_;
    std::size_t size_ = 0;
    int* data_ = nullptr;
};

int main() {
    Buffer a("a", 3);
    a.print();

    std::cout << "---- 拷贝构造 ----\n";
    Buffer b = a;
    b.set(0, 99);
    a.print();
    b.print();
    std::cout << "a 和 b 是否共享同一块内存: "
              << std::boolalpha << (a.raw_data() == b.raw_data()) << '\n';

    std::cout << "---- 移动构造 ----\n";
    Buffer c = std::move(a);
    a.print();
    c.print();
    std::cout << "a 移动后是否为空: "
              << std::boolalpha << (a.raw_data() == nullptr) << '\n';
}
```

可能输出（地址会因机器不同而变化）：

```text
构造: a 持有地址 0x12345678
a | data=0x12345678 | values=1 2 3
---- 拷贝构造 ----
拷贝构造: 从 a 深拷贝出 a_copy，新地址是 0x87654321
a | data=0x12345678 | values=1 2 3
a_copy | data=0x87654321 | values=99 2 3
a 和 b 是否共享同一块内存: false
---- 移动构造 ----
移动构造: a 接管地址 0x12345678
[已搬空] | data=0 | values=<null>
a | data=0x12345678 | values=1 2 3
a 移动后是否为空: true
```

这个类里最核心的成员不是 `size_`，而是 `data_`。  
`data_` 指向一块动态分配的内存，所以必须回答三个问题：

1. 对象销毁时，谁来释放这块内存
2. 对象复制时，是共享同一块内存，还是复制一份新内存
3. 对象移动时，旧对象要不要把所有权交出去

上面的实现给出的答案分别是：

- 析构函数负责 `delete[] data_`
- 拷贝时重新申请新内存，并复制内容
- 移动时直接转移指针所有权，并把源对象置空

## 如果只写析构，不写拷贝/移动，会怎样

这是最容易出问题的地方。

假设你写了析构函数释放 `data_`，但拷贝构造仍然使用编译器默认行为，那么默认拷贝只会把指针值复制过去：

```cpp
Buffer a(10);
Buffer b = a; // 如果是默认拷贝，这里只是复制指针地址
```

这样 `a.data_` 和 `b.data_` 就会指向同一块内存。  
最后两个对象析构时都会 `delete[]` 同一个地址，程序就会出错。

这就是为什么资源类不能只写一半：

> **一旦你自己管理资源，就必须把“销毁、复制、转移”这几个动作一起考虑。**

## 规则零、规则三、规则五

初学阶段不必死记术语，但要形成下面的直觉：

- 规则零：不自己管理原始资源时，尽量依赖默认行为
- 规则三：如果你需要自己写析构、拷贝构造、拷贝赋值，往往三个都要考虑
- 规则五：到了 `C++11`，还要继续考虑移动构造和移动赋值

所以规则三、规则五并不是“背诵题”，而是在提醒你：

> **资源所有权一旦进入类设计，对象的整个生命周期就必须自洽。**

----

# RAII

现代 `C++` 类设计的一个核心思想是 RAII：**资源在对象构造时获取，在析构时释放。**  
这意味着类的职责通常不只是保存数据，还包括稳定地管理外部资源，例如：

- 文件句柄
- 锁
- 动态内存
- 数据库连接
- 网络或系统句柄

`RAII` 把“记得释放资源”从业务流程里拿掉了，改成依赖对象生命周期自动完成。这样做的好处非常直接：

- 代码更不容易漏释放
- 遇到异常时也更安全
- 资源所有权更清晰

举个非常典型的例子：如果互斥锁通过对象构造加锁、析构解锁，那么即使函数中途 `return` 或抛异常，锁也不会遗漏释放。

所以在现代 `C++` 里，很多好设计本质上都在做同一件事：**把资源包装进对象，把清理动作放进析构。**

----

# 综合示例

下面这个例子把前面的几个点放到一起：

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

这个类虽然简单，但已经体现出比较典型的现代写法：

- 成员默认值集中描述默认状态
- 构造函数只在必要时覆盖默认值
- 成员函数通过 `const` 表达“不会修改对象”

读这类代码时，重点不只是“语法合法”，而是能否快速看出对象默认长什么样、如何被初始化、接口边界在哪里。

----

# 推荐实践

- 成员优先在初始化列表中初始化。
- 默认状态适合放在成员默认值里统一表达。
- 多个构造函数共享逻辑时，优先使用委托构造。
- 简单默认行为用 `=default` 显式表达。
- 不允许的操作用 `=delete` 显式禁止。
- 不自己管理原始资源时，优先依赖编译器默认析构和默认拷贝行为。
- 有资源管理职责的类必须认真设计拷贝和移动语义。
- 优先用对象生命周期管理资源，而不是把释放代码散落在业务流程里。

----

# 小结

类在现代 `C++` 里从来不只是“属性 + 方法”的容器。  
它更像一个边界清晰的对象单元：既要封装数据和行为，也要说明对象如何构造、如何析构、如何复制、如何移动、何时释放资源。

尤其要记住：析构函数并没有在 `C++11` 里消失，只是很多情况下已经不需要你亲手去写；  
但一旦类开始直接管理资源，就必须把析构、拷贝、移动当成同一组问题一起设计。

如果这一章能建立起“读类先看生命周期与资源语义”的习惯，后面再学习继承、多态、容器和泛型代码时，会容易很多。