# 类和对象

如果说早期 `C++` 里的类更多被当成 *把数据和函数装在一起* 的工具；  
那么到了 `C++11`，类设计已经明显升级成了另一件事：
> 它不仅要封装行为，还要表达对象的生命周期、资源所有权、可复制性、可移动性，以及接口边界。

所以学习类时，不能只停留在“会写成员函数、会写构造函数”。  
更重要的是建立一个现代直觉：**类往往同时也是资源语义和对象语义的载体。**

这一章主要回答三件事：

- 对象怎样建立边界和默认状态
- 构造、析构、拷贝、移动怎样组成一套完整语义
- 特殊成员函数怎样把设计意图写进类型

至于“拥有、借用、观察、RAII”这条资源主线，后面的 `所有权与 RAII` 会再系统收束。  
这里更偏向“从类设计角度建立对象模型”。

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

# this 指针

当我们写出下面这样的调用时：

```cpp
User user("alice");
user.print();
```

表面上看只是“对象调用成员函数”，但这里其实包含了一个很重要的事实：
> 成员函数总是在某个具体对象上执行。

`print()` 能直接访问 `name_`，不是因为成员函数“天然就能看到所有成员”，而是因为它在执行时始终对应着一个当前对象。  
这个 *当前对象*，在 `C++` 里就是通过 `this` 指针表示的。

可以先把它理解成： **`this` 指向当前正在调用这个成员函数的对象。**

```cpp
#include <iostream>
#include <string>

class User {
public:
    explicit User(const std::string& name) : name_(name) {}
    void print() const {
        std::cout << this->name_ << std::endl;
    }
private:
    std::string name_;
};
```

> 这里的 `this->name_` 和直接写 `name_` 在效果上是一样的。  

编译器知道 `print()` 是在某个 `User` 对象上调用的，所以它会把成员访问理解为“访问当前对象的成员”。  
因此，很多时候我们并不会把 `this->` 显式写出来。

### 为什么要有 this

同一个成员函数可以被很多对象共用，但每次调用时，它都必须知道自己正在处理哪一个对象。

```cpp
User u1("alice");
User u2("bob");

u1.print();
u2.print();
```

调用的是同一个 `print()`，但输出结果不同。根本原因不是函数代码变了，而是每次调用时 `this` 指向的对象不同：

- 调用 `u1.print()` 时，`this` 指向 `u1`
- 调用 `u2.print()` 时，`this` 指向 `u2`

所以可以把成员函数粗略理解成：**它比普通函数多了一个隐藏的“当前对象”参数。**

### C++11 里应该怎样理解它

在 `C++11` 的语境下，`this` 最值得掌握的不是“底层语法细节”，而是下面几个用途：

- 它让成员函数和具体对象绑定起来
- 它让 `const` 成员函数能够表达“我不会修改当前对象”
- 它支持返回当前对象本身，例如 `return *this;`
- 它支持判断是否是`同一个对象`，例如 `this == &other`

这几个用途会在类设计里反复出现，尤其是在拷贝赋值、移动赋值、链式调用这些地方。

### const 成员函数和 this

```cpp
class User {
public:
    void set_name(const std::string& name) {
        name_ = name;
    }
    void print() const {
        std::cout << name_ << std::endl;
    }
private:
    std::string name_;
};
```

`set_name()` 可以修改对象状态，而 `print() const` 不可以。从 `this` 的角度看，可以把它们粗略理解成：

- 普通成员函数里，`this` 表示“指向当前对象”
- `const` 成员函数里，`this` 表示“指向当前常量对象”

所以 `print() const` 里不能随意改 `name_`，因为这等价于“通过当前对象去修改对象状态”，而 `const` 已经禁止了这种行为。

> 这也是 `const` 成员函数真正重要的地方：它不是修饰返回值，而是在约束“当前对象是否允许被修改”。

### 什么时候会显式写 this

初学阶段不需要在每个成员访问前都写 `this->`。在 `C++11` 的日常代码里，显式写出 `this` 通常有几种常见情况：

- 需要强调“这是当前对象的成员”
- 成员名和参数名重名时，用 `this->name_` 区分
- 需要返回当前对象本身时，写 `return *this;`
- 需要判断是不是同一个对象时，写 `this == &other`

比如：

```cpp
class User {
public:
    User& set_name(const std::string& name) {
        this->name_ = name;
        return *this;
    }

private:
    std::string name_;
};
```

这里有两个很典型的写法：

- `this->name_`：表示修改当前对象的成员
- `return *this;`：返回当前对象本身，从而支持链式调用

后面讲拷贝赋值和移动赋值时，你还会看到：

```cpp
if (this != &other) {
    // 避免自己给自己赋值
}
```

这时 `this` 的作用就非常直观了：拿 `当前对象的地址` 去和 `另一个对象的地址` 比较。

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

# 构造函数

构造函数用于在对象创建时建立初始状态。  
它的名字和类名相同，没有返回类型，也不能手动像普通成员函数那样随便调用。

可以把它理解成：

- 对象“出生”的入口
- 成员拿到初始值的主要位置
- 类型设计意图最集中的地方之一

> 如果一个类的构造函数设计得清楚，那么这个类通常也会更容易被正确使用。

最简单的构造函数写法如下：

```cpp
class User {
public:
    User() {}
};
```

这里的 `User()` 就是构造函数。当你写出 `User user;` 时，这个构造函数就会被调用。  
构造函数最常见的职责是：

- 初始化成员
- 建立对象的不变量
- 保证对象一创建出来就是可用状态

所谓“不变量”，可以简单理解成“这个对象在整个生命周期中必须满足的基本规则”。  
例如一个 `Date` 对象可能要求月份始终在 `1` 到 `12` 之间，一个 `Socket` 对象可能要求要么处于有效连接状态，要么明确处于未连接状态。

## 默认构造函数

> 不需要参数就能调用的构造函数，通常叫默认构造函数。

```cpp
class User {
public:
    User() {}
};
int main() {
    User user;
}
```

默认构造函数常用于表达 **这个类型存在一个合理的默认状态**。比如：

- 字符串默认为空串
- 数值默认为 `0`
- 配置对象先给出一组常用默认配置

> 如果一个类声明了其它构造函数，编译器通常就不会再自动帮你生成无参构造函数了。  

这也是为什么下面的写法不能默认构造：

```cpp
class User {
public:
    explicit User(int id) : id_(id) {}
private:
    int id_;
};
int main() {
    // User user;   // 错误：没有默认构造函数
    User user(1);
}
```

所以在设计类型时，应该先想清楚一件事：*这个类是否真的应该允许“无参数地被创建”？*  
- 如果答案是“应该”，那就提供一个明确的默认构造函数；
- 如果答案是“不应该”，那就不要勉强给它造一个没有意义的默认状态。


## 初始化列表

成员对象通常应通过初始化列表初始化，而不是先默认构造再赋值：

```cpp
class Point {
public:
    Point(int x, int y) : x_(x), y_(y) {
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
    Config(int port, std::string& name) : port_(port), name_(name) {}
private:
    const int port_;
    std::string& name_;
};
```

> 可以把初始化列表理解成“对象诞生时就把成员放到正确状态”，而不是“先生出来，再慢慢修正”。

!> 当一个构造函数只接受一个参数时，通常要考虑是否加 `explicit`，避免隐式转换

### 初始化顺序

这是构造函数里非常容易写错、也非常值得单独强调的一点。对象创建时，大致顺序可以理解为：
1. 先初始化基类部分
2. 再按成员声明顺序初始化成员
3. 最后才进入构造函数函数体

其中最容易误解的是第二步：**成员的实际初始化顺序，取决于类里声明的顺序，而不是初始化列表里的顺序。**

```cpp
class Demo {
public:
    Demo() : b_(a_), a_(10) {}
private:
    int a_;
    int b_;
};
```

很多人第一眼会以为先执行 `b_(a_)`，再执行 `a_(10)`。  
实际上并不是这样。因为成员声明顺序是：先 `a_` 再 `b_`  

> 也正因如此，初始化列表的书写顺序最好和成员声明顺序保持一致。  
> 这样不但更容易读，也能减少警告和误解。

## 成员默认值

`C++11` 允许在类定义处直接给成员提供默认值：**这会让类的默认状态更集中、更清楚。**

```cpp
#include <string>

class Config {
private:
    int port_ = 8080;
    std::string host_ = "127.0.0.1";
};
```

如果没有成员默认值，很多构造函数都要重复写相同的初始化逻辑。  
一旦构造函数数量变多，这类重复代码很容易让默认状态分散在多个位置。

成员默认值的好处是：
- 默认状态写在成员定义旁边，读起来直观
- 多个构造函数可以共享默认行为
- 只有少数特殊构造函数才需要覆盖默认值

```cpp
#include <iostream>
#include <string>

class User {
public:
    User() = default;
    User(int id, const std::string &name) : id_(id), name_(name) {}
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

需要注意的是：

- 如果构造函数的初始化列表里显式写了某个成员，那么它会覆盖成员默认值
- 如果没有显式写，成员默认值才会生效

## 构造函数重载

> 一个类往往不止一种创建方式，因此构造函数也经常会重载：

```cpp
class User {
public:
    User() : id_(0), name_("guest") {}
    User(int id) : id_(id), name_("guest") {}
    User(int id, const std::string& name) : id_(id), name_(name) {}
private:
    int id_;
    std::string name_;
};
```

这表示同一个类型可以有多个“构造入口”：

- 完全使用默认值
- 只指定关键参数
- 一次性给出完整参数

重载本身不难，真正难的是保持这些构造入口之间的语义一致。  
如果一个对象在不同构造路径下呈现出完全不同的默认规则，使用者就会很难预测它的行为。

因此设计构造函数重载时，通常应遵循两个原则：

- 不同重载之间要有清晰分工
- 默认行为尽量集中，不要在多个构造函数里重复散落

这里先从“对象如何被创建”理解构造重载。  
更系统的重载规则、隐式转换风险和匹配行为，放在后面的 `函数重载` 一章里。

### 委托构造

> ​​委托构造是建立在构造函数重载基础之上的一种“代码复用”机制。​

```cpp
#include <string>

class User {
public:
    User() : User("guest") {}
    explicit User(const std::string& name) : name_(name) {}
private:
    std::string name_;
};
```

委托构造的核心价值就是把“真正的初始化逻辑”集中到一个主构造函数里，让其它构造函数只是转发参数。

> 这和普通函数里“提取公共逻辑”的思路很像，只不过这里提取的是对象初始化逻辑。

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
#include <string>

class Test {
public:
    Test(const std::string& n) : name(n) {
        std::cout << "构造: " << name << std::endl;
    }
    ~Test() {
        std::cout << "析构: " << name << std::endl;
    }
private:
    std::string name;
};

void myFunction() {
    Test a1("a1");
    Test a2("a2");
    Test a3("a3");
} // 退出函数时，自动触发析构

int main() {
    std::cout << "--- 进入函数 ---" << std::endl;
    myFunction();
    std::cout << "--- 退出函数 ---" << std::endl;
    return 0;
}
```

> 在 C++ 中，局部对象的析构顺序与它们的构造顺序​​严格相反​​。

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

如果你想把这件事放到“所有权”和“生命周期”主线上统一理解，可以继续看后面的 `所有权与 RAII`。

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
    User(int id, const std::string& name) : id_(id), name_(name) {}
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

# 拷贝和移动

这是 `C++11` 类设计里最重要的升级之一。  
> 学习本节之前一定要先学 [《引用》](advanced/ref.md) 作为基础知识。

很多初学者一看到“拷贝构造、移动构造、拷贝赋值、移动赋值”就头大，根本原因通常不是语法太难，而是没有先想清楚：
> 这个对象到底“拥有”什么，它被复制或转移时应该发生什么？

这里主要从“类应该如何表现”来理解它们。  
值类别、`std::move`、借用语义这些前置概念，仍然以 `引用` 一章为主。

一句话理解：
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
    explicit Buffer(std::string name) : name_(std::move(name)) {}

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

这里只需要先抓住一个结论：
- `b2 = b1` 表示“我想复制一份”
- `std::move(b1)` 表示“我允许把 b1 当成可被搬走资源的对象”

> 注意：`std::move` 本身并不移动资源，它只是把对象转换成“可以触发移动语义”的形式。  
> 真正怎么移动，要看你的移动构造/移动赋值如何实现。

## 资源类必须区分拷贝和移动

如果类里只是 `int`、`std::string`、`std::vector` 这类成员，很多时候默认行为已经够用。  
但如果类自己直接管理原始资源，例如一块动态内存，那事情就不能糊弄过去了。

> 下面这个例子一定要仔细阅读调试，同时把“构造，析构、拷贝构造、拷贝赋值、移动构造、移动赋值”都串起来

```cpp
#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

class Buffer {
public:
    // 普通构造
    Buffer(std::string name, std::size_t size)
        : name_(std::move(name)), size_(size), data_(size ? new int[size] : nullptr) {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = static_cast<int>(i + 1);
        }
    }
    // 析构
    ~Buffer() {
        std::cout << "析构: " << name_ << " 释放地址 " << static_cast<void*>(data_) << '\n';
        delete[] data_;
    }
    // 拷贝构造
    Buffer(const Buffer& other) {
        copy_from(other, "_cp");
    }
    // 拷贝赋值
    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            delete[] data_;
            copy_from(other, "_cp_assign");
        }
        return *this;
    }
    // 移动构造
    Buffer(Buffer&& other) noexcept {
        take_from(other);
    }
    // 移动赋值
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            take_from(other);
        }
        return *this;
    }

    void set(std::size_t index, int value) {
        if (data_ != nullptr && index < size_) data_[index] = value;
    }

    const int* raw_data() const { return data_; }

    void print(std::string prefix = "") const {
        std::cout << prefix << " : " << std::left
                  << "name: " << std::setw(20) << name_
                  << "size: " << std::setw(6) << size_
                  << "data地址: " << std::setw(18) << static_cast<const void*>(data_)
                  << "data内容: ";
        if (data_ == nullptr) {
            std::cout << "<null>\n";
            return;
        }

        for (std::size_t i = 0; i < size_; ++i) std::cout << data_[i] << ' ';
        std::cout << '\n';
    }

private:
    void copy_from(const Buffer& other, const char* suffix) {
        name_ = other.name_ + suffix;
        size_ = other.size_;
        data_ = size_ ? new int[size_] : nullptr;
        if (size_ > 0) std::copy(other.data_, other.data_ + size_, data_);
    }

    // 注意：这里使用 Buffer& other 既可接受左值，也可接受右值
    // 如果一定要改成 Buffer&& other，在调用时需要显式使用 std::move(other)
    void take_from(Buffer& other) {
        name_ = std::move(other.name_);
        size_ = other.size_;
        data_ = other.data_;
        other.name_ = "[moved]";
        other.size_ = 0;
        other.data_ = nullptr;
    }

    std::string name_;
    std::size_t size_ = 0;
    int* data_ = nullptr;
};

int main() {
    Buffer a("A", 3);           // 普通构造*
    Buffer b = a;               // 拷贝构造，b 是 a 的副本
    Buffer c("C", 1);           // 普通构造*

    std::cout << "---- a, b, c 构造完成后 ----" << std::endl;
    a.print("a");
    b.print("b");
    c.print("c");

    c = b;                      // 拷贝赋值，c 被赋值为 b 的副本
    std::cout << "---- c = b 赋值完成后 ----" << std::endl;
    b.print("b");
    c.print("c");

    Buffer d = std::move(a);    // 移动构造，d 接管了 a 的资源
    std::cout << "---- d = std::move(a) 移动构造完成后 ----" << std::endl;
    a.print("a");
    d.print("d");

    Buffer e("E", 1);           // 普通构造*
    std::cout << "---- e 构造完成后 ----" << std::endl;
    e.print("e");
    e = std::move(c);           // 移动赋值，e 接管了 c 的资源
    std::cout << "---- e = std::move(c) 移动赋值完成后 ----" << std::endl;
    c.print("c");
    e.print("e");

    std::cout << "---- b.set(0, 99) 和 e.set(1, 88) 设置完成后 ----" << std::endl;
    b.set(0, 99);
    e.set(1, 88);
    b.print("b");
    e.print("e");

    return 0;
}

```

## 析构与拷贝/移动的关系

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
- **规则三：如果你需要自己写析构、拷贝构造、拷贝赋值，往往三个都要考虑**
- 规则五：到了 `C++11`，还要继续考虑移动构造和移动赋值

所以规则三、规则五并不是“背诵题”，而是在提醒你：**资源所有权一旦进入类设计，对象的整个生命周期就必须自洽。**

----

# RAII

这一节只保留一个和类设计最相关的结论：

> 当一个类开始承担资源管理职责时，它通常就不只是“装数据的对象”，而是一个 `RAII` 类型。

也就是说：

- 构造函数负责把资源拿到手
- 析构函数负责把资源稳定释放
- 拷贝和移动负责定义资源如何复制或转移

如果你现在已经能把这三件事连起来，这一章的目标就达到了。  
更系统的 `拥有 / 借用 / 观察` 分类，以及 `RAII` 在工程代码里的展开，放在后面的 `所有权与 RAII` 一章里。

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
- 资源关系一旦变复杂，就结合 `所有权与 RAII` 一章一起看。

----

# 小结

类在现代 `C++` 里从来不只是“属性 + 方法”的容器。  
它更像一个边界清晰的对象单元：既要封装数据和行为，也要说明对象如何构造、如何析构、如何复制、如何移动、何时释放资源。

尤其要记住：析构函数并没有在 `C++11` 里消失，只是很多情况下已经不需要你亲手去写；  
但一旦类开始直接管理资源，就必须把析构、拷贝、移动当成同一组问题一起设计。

如果这一章能建立起“读类先看生命周期与资源语义”的习惯，后面再学习继承、多态、容器和泛型代码时，会容易很多。