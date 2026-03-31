# 继承

继承是 `C++` 面向对象里最经典的话题之一，但现代 `C++` 对继承的态度比过去克制得多。`C++11` 没有改变继承的基本语法，却通过 `override`、`final` 等工具，让“继承到底是不是合理设计”这件事变得更容易表达，也更值得认真判断。

在真实项目里，继承不是默认复用手段，而是一种带有强语义承诺的关系。只要用上继承，就等于在说：“派生类本质上就是基类的一种。”

----

# 基本继承

```cpp
#include <iostream>

class Animal {
public:
    void eat() const {
        std::cout << "eat" << std::endl;
    }
};

class Dog : public Animal {
};

int main() {
    Dog dog;
    dog.eat();
    return 0;
}
```

这段代码说明派生类会继承基类可访问的成员能力。在 `public` 继承下，`Dog` 可以直接使用 `Animal` 暴露出来的公共接口。

## 先理解继承表达的不是“复用”，而是“关系”

初学者很容易把继承理解成“少写点代码的办法”。  
但更准确的理解是：继承首先表达的是类型关系，而不是代码复用技巧。

所以在决定是否继承前，最关键的问题通常不是“能不能复用那几个成员函数”，而是：

- `Dog` 是不是一种 `Animal`
- 调用方能不能把 `Dog` 当 `Animal` 使用
- 这个关系在设计上是否稳定

如果这三个问题都答得不自然，就要小心：  
你可能并不是在建模“继承关系”，而只是想借用几段现成代码。

----

# 继承方式

语法上可以写：

- `public` 继承
- `protected` 继承
- `private` 继承

其中最常见、最符合 “is-a” 关系的是 `public` 继承。

## 三种继承方式大致怎么理解

- `public` 继承：向外部保留“是一种”的接口语义
- `protected` 继承：更像给派生类复用的实现机制，较少直接作为公开建模使用
- `private` 继承：更像“用实现细节”的特殊形式，很多场景下组合通常更清晰

所以大多数面向对象建模里，你真正最常用、也最值得重点掌握的是 `public` 继承。

## 继承后成员可见性会发生什么

很多初学者会把“继承方式”和“成员访问级别”混在一起。  
这两个概念有关，但不是一回事：

- 成员本身原来是 `public`、`protected` 还是 `private`
- 派生时又用了 `public`、`protected` 还是 `private` 继承

最需要先记住的是 `public` 继承下的直觉：

- 基类 `public` 成员，在派生类外部仍然按 `public` 使用
- 基类 `protected` 成员，派生类内部可以用，但外部不能直接用
- 基类 `private` 成员，不会直接暴露给派生类访问

例如：

```cpp
#include <iostream>

class Base {
public:
    void show() const {
        std::cout << "public member" << std::endl;
    }

protected:
    int value_ = 10;

private:
    int hidden_ = 20;
};

class Derived : public Base {
public:
    void print() const {
        show();                    // 可以，基类 public 成员
        std::cout << value_ << std::endl; // 可以，基类 protected 成员
        // std::cout << hidden_ << std::endl; // 不可以，基类 private 成员
    }
};
```

这个规则的意义在于：  
派生类继承的是基类的公开接口和一部分可复用实现，但**不会越过 `private` 边界直接接管基类内部细节**。

----

# 构造析构顺序

派生类对象创建时：

1. 先构造基类部分
2. 再构造派生类部分

销毁时顺序相反。

```cpp
#include <iostream>

class Base {
public:
    Base() { std::cout << "Base ctor" << std::endl; }
    ~Base() { std::cout << "Base dtor" << std::endl; }
};

class Derived : public Base {
public:
    Derived() { std::cout << "Derived ctor" << std::endl; }
    ~Derived() { std::cout << "Derived dtor" << std::endl; }
};
```

## 为什么必须先基类后派生类

因为派生类对象内部本来就包含一个基类子对象。  
只有先把基类部分构造好，派生类才能在一个完整的基础上继续初始化自己。

销毁反过来也是同理：

- 先销毁派生类自己新增的部分
- 再销毁更基础的基类部分

理解这个顺序后，很多构造和析构相关问题就会更容易推理。

## 运行一下会看到什么

如果创建一个 `Derived` 对象，输出顺序通常是：

```text
Base ctor
Derived ctor
Derived dtor
Base dtor
```

也就是：

- 创建时由内到外打基础
- 销毁时由外到内逐层回收

这和“派生类建立在基类之上”的结构本身是一致的。

----

# 基类构造

```cpp
#include <string>

class Animal {
public:
    explicit Animal(const std::string& name) : name_(name) {
    }

protected:
    std::string name_;
};

class Dog : public Animal {
public:
    Dog(const std::string& name, int age)
        : Animal(name), age_(age) {
    }

private:
    int age_;
};
```

如果基类没有默认构造函数，派生类就必须在初始化列表中明确调用合适的基类构造函数。

## 这里最容易忽略什么

很多人写派生类构造函数时，只关注自己新增的成员，却忘了基类部分也需要被正确初始化。  
在继承体系里，派生类的构造函数不只是初始化“自己的字段”，还要负责把基类子对象放到正确状态。

所以阅读派生类构造函数时，建议优先看两件事：

- 它调用了哪个基类构造函数
- 这个选择是否符合派生类的语义

## 不要以为“先写谁就先构造谁”

派生类初始化列表里即使把自己的成员写在前面，基类仍然会先构造。  
因为顺序不是由你写初始化列表的先后决定的，而是由对象结构本身决定的：

1. 先构造基类子对象
2. 再构造成员对象
3. 最后进入派生类构造函数体

所以初始化列表最主要的职责，是**说明怎么初始化**，而不是**改变构造顺序**。

----

# 继承中的析构

前一章已经讲过析构函数，这里要强调继承体系里最关键的一条额外规则。

如果一个类会被当成基类，并且对象可能通过基类指针删除，那么基类析构函数应当是 `virtual`：

```cpp
class Animal {
public:
    virtual ~Animal() = default;
};
```

## 为什么这里必须小心

看下面这个例子：

```cpp
#include <iostream>

class Base {
public:
    ~Base() {
        std::cout << "Base dtor" << std::endl;
    }
};

class Derived : public Base {
public:
    ~Derived() {
        std::cout << "Derived dtor" << std::endl;
    }
};

int main() {
    Base* ptr = new Derived();
    delete ptr; // 风险点
}
```

如果基类析构函数不是虚函数，那么这里的 `delete ptr;` 可能只调用 `Base` 的析构，而不调用 `Derived` 的析构。

这会带来两个问题：

- 派生类自己的清理逻辑没有执行
- 派生类管理的资源可能泄漏

所以这条经验非常重要：

> **只要一个类打算作为多态基类使用，就优先把析构函数写成虚函数。**

这不是因为“基类一定有资源”，而是因为“销毁动作可能发生在只看得到基类接口的地方”。

----

# `override`

虽然 `override` 常放在多态章节里讲，但在继承层面它同样非常关键。它能明确表示“这个函数是在覆盖基类虚函数”。

```cpp
class Base {
public:
    virtual void run() const {
    }
};

class Derived : public Base {
public:
    void run() const override {
    }
};
```

如果函数签名不匹配，编译器会直接报错，这比旧时代“悄悄没覆盖成功”安全得多。

## 为什么继承体系里要把它当默认写法

继承一旦涉及虚函数，最怕的就是“看起来像重写，实际上只是新增了一个同名函数”。  
`override` 可以让这种错误在编译期暴露，而不是拖到运行时行为异常才发现。

所以一个很稳的习惯是：

- 只要你是想覆盖基类虚函数
- 就始终写上 `override`

## 覆盖和同名隐藏不是一回事

这是继承里非常容易混淆的一点。

如果基类函数不是虚函数，或者派生类函数签名不同，那么你写出一个同名函数，并不代表真正“覆盖”了基类行为。  
很多时候它只是把基类里的同名函数隐藏了。

所以判断标准不是“名字像不像”，而是：

- 基类函数是不是 `virtual`
- 派生类函数签名是否匹配
- 是否明确写了 `override`

----

# `final`

`C++11` 支持禁止进一步继承：

```cpp
class Base final {
};
```

也可以禁止某个虚函数继续被覆盖：

```cpp
class Base {
public:
    virtual void run() final {
    }
};
```

## 它在设计上表达了什么

`final` 的价值不只是“防止别人继续写子类”，更是在表达设计边界：

- 这个类型的继承层次到这里结束
- 这个虚函数的行为不希望再被派生类改写

当一个类的扩展边界已经稳定，或者继续继承会破坏语义时，`final` 是很直接的表达方式。

----

# 继承与组合

接手现代项目时，经常会看到“组合优于继承”的建议。原因不是继承不好，而是它容易导致：

- 层次过深
- 基类职责膨胀
- 派生类过度依赖基类实现细节
- 修改一个基类影响大量子类

如果两个类只是“使用关系”而非真正的“是一种”，优先考虑组合。

## 先问是不是 `is-a`

一个实用判断方式是先问：

- 这个派生类是否真的能被自然地当成基类使用
- 这种关系是否稳定，而不是暂时凑巧
- 我想复用的是语义，还是只是几段实现代码

如果答案更接近“只是想复用一点能力”，那通常组合会更合适。

例如：

- `UserService` 有一个 `Logger`
- `OrderProcessor` 有一个 `Storage`

这类关系通常不是继承，而是依赖或组合。

## 一个很实用的判断口诀

可以把这个判断压缩成一句话：

> **想表达“是一个”，再考虑继承；想表达“有一个”或“用一个”，优先组合。**

例如：

- `Dog` 是一个 `Animal`
- `Car` 有一个 `Engine`
- `UserService` 用一个 `Logger`

前者更像继承关系，后两者通常更像组合关系。

----

# 综合示例

```cpp
#include <iostream>
#include <string>

class Animal {
public:
    explicit Animal(const std::string& name) : name_(name) {
    }

    virtual ~Animal() = default;

    virtual void speak() const {
        std::cout << "animal" << std::endl;
    }

protected:
    std::string name_;
};

class Dog : public Animal {
public:
    explicit Dog(const std::string& name) : Animal(name) {
    }

    void speak() const override {
        std::cout << name_ << " says woof" << std::endl;
    }
};

int main() {
    Dog dog("buddy");
    dog.speak();
    return 0;
}
```

这个例子里有几层信息值得注意：

- `Dog` 通过 `public` 继承表达自己是一种 `Animal`
- 派生类构造函数先初始化基类部分
- 基类提供虚析构，保证通过基类接口销毁时行为正确
- `speak()` 用 `override` 明确表达覆盖关系

这比单纯把“能复用的代码”塞进基类，更像是在明确描述类型模型。

----

# 推荐实践

- 真正表达 `is-a` 关系时才使用 `public` 继承。
- 继承前先判断自己是不是在做语义建模，而不是只想复用几行代码。
- 先分清“成员访问级别”和“继承方式”不是同一件事。
- 只要是在重写虚函数，就加 `override`。
- 确认类型不应该再被继承时，考虑 `final`。
- 有虚函数的继承体系里，优先检查基类是否需要虚析构。
- 没有明确抽象边界时，优先考虑组合，再考虑继承。

----

# 小结

继承在现代 `C++` 里依然重要，但它不再是默认复用手段，而是一种需要谨慎使用的类型关系。  
如果你能把“先问是不是 `is-a`”“分清访问控制和继承方式”“理解构造析构顺序”“覆盖时写 `override`”“多态基类要考虑虚析构”“不合适时优先组合”这些判断建立起来，继承这章就算真正理解到位了。
