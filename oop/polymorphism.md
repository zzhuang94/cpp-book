# 多态

多态是 `C++` 面向对象里最核心的能力之一。很多人第一次接触它时，只记住了“基类指针指向派生类对象，调用虚函数会动态分派”。但在现代 `C++` 里，这只是起点。

真正重要的是另外几件事：哪些地方值得引入运行时多态，如何保证销毁安全，如何避免对象切片，以及为什么 `override` 和智能指针几乎成了多态代码的标配。

----

# 运行时多态

最常见的是运行时多态，也就是通过基类接口调用派生类实现：

```cpp
#include <iostream>

class Animal {
public:
    virtual void speak() const {
        std::cout << "animal" << std::endl;
    }
};

class Dog : public Animal {
public:
    void speak() const override {
        std::cout << "woof" << std::endl;
    }
};
```

这里 `virtual` 的作用是告诉编译器：这个调用点不要只看变量的静态类型，还要在运行时根据对象真实类型决定调用哪个版本。

## 多态到底解决什么问题

它解决的不是“语法看起来高级”，而是扩展点问题。  
当调用方只需要一组稳定能力，而具体实现可能有多个版本时，多态就能把“如何做”交给具体类型自己决定。

这意味着调用方关注的是：

- 这个对象会不会 `speak()`
- 它是否满足抽象接口约定

而不是：

- 它到底是 `Dog`
- 还是 `Cat`
- 还是以后新增的别的动物类型

所以多态真正带来的价值，不是“少写 `if/else`”这么简单，而是：

> **让调用方依赖稳定抽象，而不是依赖具体实现。**

----

# 静态类型与动态类型

要真正看懂多态，先要分清两个词：

- 静态类型：代码里变量声明出来的类型
- 动态类型：这个变量运行时实际指向或引用的对象类型

例如：

```cpp
Animal* animal = new Dog();
```

这里：

- `animal` 的静态类型是 `Animal*`
- 它指向对象的动态类型是 `Dog`

当 `speak()` 是虚函数时，调用会根据动态类型决定，因此最终调用的是 `Dog::speak()`。

如果没有这个区分，很多人就会疑惑：

> 变量明明写的是 `Animal`，为什么最后执行的是 `Dog` 的版本？

答案正是：多态看的是对象真实类型，而不是只看变量声明时写的名字。

----

# 基类多态

真正体现多态价值的，是“通过基类接口使用对象”：

```cpp
#include <iostream>

class Animal {
public:
    virtual void speak() const {
        std::cout << "animal" << std::endl;
    }
};

class Cat : public Animal {
public:
    void speak() const override {
        std::cout << "meow" << std::endl;
    }
};

void makeSpeak(const Animal& animal) {
    animal.speak();
}

int main() {
    Cat cat;
    makeSpeak(cat);
    return 0;
}
```

这里如果没有 `virtual`，调用的就是静态绑定版本，也就是仅按 `Animal` 的接口实现来处理。

## 为什么通常用引用或指针

因为多态的前提，是调用点仍然保留“对象的真实类型信息”。  
使用引用、指针或智能指针时，这个动态类型信息会被保留下来；而一旦按值传递或按值存储，就很容易丢失派生类部分。

## 为什么多态接口常长这样

工程里你会经常看到下面几种函数签名：

```cpp
void draw(const Shape& shape);
void reset(Shape* shape);
void add(std::unique_ptr<Shape> shape);
```

它们虽然形式不同，但都在做同一件事：  
**通过基类接口操作对象，同时保留对象真实类型。**

这也是为什么多态代码里“按值接收基类对象”通常值得立刻警觉。

----

# 纯虚函数与抽象类

很多多态基类其实不是为了“自己也能直接创建对象”，而是为了定义一组统一接口。

这时常会把函数写成纯虚函数：

```cpp
class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
};
```

这里的 `= 0` 表示这是纯虚函数。

一旦类里有纯虚函数，这个类就成为抽象类：

- 不能直接创建对象
- 只能作为接口或基类使用
- 派生类必须实现这些纯虚函数，才能成为可实例化类型

例如：

```cpp
class Circle : public Shape {
public:
    void draw() const override {
    }
};
```

## 为什么抽象类在多态里很常见

因为多态最想表达的，往往不是“这些对象长得像”，而是：

> **这些对象都承诺提供同一组行为。**

抽象类正适合做这种“能力边界”的定义。

----

# 虚析构函数

只要类打算作为基类并通过基类指针删除派生对象，就应提供虚析构函数：

```cpp
class Base {
public:
    virtual ~Base() = default;
};
```

否则可能只调用基类析构，派生类资源无法正确释放。

## 为什么它几乎是多态基类的默认配置

多态代码里，销毁往往发生在“只看得到基类接口”的地方。  
如果析构函数不是虚的，删除动作就可能停留在基类层面，导致派生对象没有被完整销毁。

所以这条经验几乎可以记成条件反射：

- 只要一个类要作为多态基类使用
- 就优先给它虚析构函数

这和“基类自己有没有资源”并不是一回事，它表达的是销毁语义，而不是资源多少。

## 一个更完整的危险示例

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
    delete ptr; // 如果 Base 析构不是虚函数，Derived 析构可能不会执行
}
```

这也是为什么很多人把这条规则背成一句短话：

> **多态基类，析构优先写成 `virtual ~Base() = default;`**

----

# `override`

`C++11` 的 `override` 是多态代码质量的关键工具：

```cpp
class Base {
public:
    virtual void run(int value) {
    }
};

class Derived : public Base {
public:
    void run(int value) override {
    }
};
```

如果你不小心写成：

```cpp
void run(double value);
```

没有 `override` 时，这很可能只是新增了一个同名函数；加了 `override`，编译器会立即告诉你签名不匹配。

## 它的重要性为什么这么高

因为多态最怕“你以为自己覆盖成功了，其实没有”。  
这类错误肉眼很难及时发现，但 `override` 可以把它提前变成编译错误。

所以在现代 `C++` 里，一条非常稳妥的实践是：  
**只要是在重写虚函数，就始终写 `override`。**

## `virtual` 和 `override` 分工不同

它们经常一起出现，但职责并不一样：

- `virtual`：声明“这个函数支持动态绑定”
- `override`：声明“这个函数是在覆盖基类虚函数”

通常一个清晰的写法是：

- 在基类里写 `virtual`
- 在派生类里写 `override`

----

# 对象切片

这是很多初学者和多年未写 `C++` 的开发者都容易踩的坑。

```cpp
#include <iostream>

class Animal {
public:
    virtual void speak() const {
        std::cout << "animal" << std::endl;
    }
};

class Dog : public Animal {
public:
    void speak() const override {
        std::cout << "dog" << std::endl;
    }
};

void test(Animal animal) {
    animal.speak();
}
```

如果把 `Dog` 以值传递给 `test()`，派生类部分会被切掉，多态失效。这就是对象切片。

## 什么时候要立刻警觉

只要你在多态体系里看到下面两种情况，就应该立刻想到切片风险：

- 按值传递基类对象
- 按值存放基类对象

现代代码里，多态对象通常更适合：

- `Animal&`
- `const Animal&`
- `Animal*`
- 智能指针

## 为什么叫“切片”

因为按值传递时，传进去的不是完整的 `Dog` 对象，而是被“切”成了 `Animal` 那一部分。

也就是说：

- 基类子对象被拷贝进函数参数
- 派生类新增的那部分信息丢失了
- 虚调用赖以分派的真实对象身份也就没了

所以切片问题本质上不是“语法怪”，而是：

> **你把一个本来要按多态使用的对象，硬生生变成了一个普通基类值对象。**

----

# 动态绑定成本

虚函数通常通过虚函数表实现，会带来：

- 一次间接调用
- 对象布局中额外的虚表指针

这些成本是真实存在的，但在大多数业务代码里通常完全可以接受。

## 不要把“性能担忧”放在设计前面

很多人一提到虚函数，第一反应就是“会不会慢”。  
更合理的思路应该是：

1. 先判断这里是否真的需要稳定扩展点
2. 再判断多态带来的设计收益是否成立
3. 最后在有性能证据时再考虑是否需要替代方案

不要在没有证据的情况下，为了想象中的极致性能回避本来合理的多态设计。

----

# 智能指针

现代 `C++` 中，多态对象常与智能指针配合：

```cpp
#include <iostream>
#include <memory>

class Animal {
public:
    virtual ~Animal() = default;
    virtual void speak() const = 0;
};

class Dog : public Animal {
public:
    void speak() const override {
        std::cout << "woof" << std::endl;
    }
};

int main() {
    std::unique_ptr<Animal> animal = std::make_unique<Dog>();
    animal->speak();
    return 0;
}
```

这比手动 `new/delete` 更安全，也更能清楚表达对象所有权。

## 为什么多态对象常和智能指针一起出现

因为多态对象经常需要：

- 在堆上创建
- 通过基类接口长期保存
- 在多个作用域之间转移或共享生命周期

这时智能指针就把“谁负责释放对象”这件事表达得更明确，也能减少手动管理资源的错误。

如果是独占所有权，`std::unique_ptr` 通常最自然；  
如果确实需要共享生命周期，再考虑 `std::shared_ptr`。

----

# 综合示例

```cpp
#include <iostream>
#include <memory>
#include <vector>

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
};

class Circle : public Shape {
public:
    void draw() const override {
        std::cout << "draw circle" << std::endl;
    }
};

class Rectangle : public Shape {
public:
    void draw() const override {
        std::cout << "draw rectangle" << std::endl;
    }
};

int main() {
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>());
    shapes.push_back(std::make_unique<Rectangle>());

    for (const auto& shape : shapes) {
        shape->draw();
    }

    return 0;
}
```

这个例子把多态最关键的几个点串在了一起：

- 用抽象基类定义稳定能力边界
- 用 `override` 保证派生类覆盖关系正确
- 用虚析构保证通过基类接口销毁时安全
- 用 `unique_ptr` 管理对象生命周期
- 用容器统一保存不同具体类型

这也是工程里非常常见的一种多态组织方式。

----

# 推荐实践

- 只有真的需要运行时扩展点时才引入多态。
- 作为基类使用的类型通常应有虚析构函数。
- 重写虚函数时始终加 `override`。
- 抽象基类优先只定义必要接口，不要顺手塞进过多实现细节。
- 多态接口优先通过引用、指针或智能指针传递。
- 注意对象切片，不要随手按值传递或按值存放基类对象。
- 纯接口类优先只暴露必要虚函数。

----

# 小结

多态的核心不只是“虚函数能动态分派”，而是“如何在继承体系里安全、明确地设计扩展点”。  
只要你能同时抓住静态类型与动态类型、抽象基类、虚析构、`override`、切片风险和生命周期管理这几个关键点，多态这一章就已经从语法理解走到工程理解了。
