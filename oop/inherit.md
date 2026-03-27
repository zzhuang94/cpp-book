# 继承

继承是 `C++` 面向对象里最经典的话题之一，但现代 `C++` 对继承的态度比过去克制得多。`C++11` 没有改变继承的基本语法，却提供了 `override`、`final` 等更明确的表达方式，也让“何时不该继承”这件事更值得认真考虑。

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

# 继承方式

语法上可以写：

- `public` 继承
- `protected` 继承
- `private` 继承

其中最常见、最符合“is-a” 关系的是 `public` 继承。

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

# 基类构造

```cpp
#include <iostream>
#include <string>

class Animal {
public:
    Animal(const std::string& name) : name_(name) {
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

# `override`

虽然 `override` 常放在多态章节里讲，但在继承层面它非常关键。它能明确表示“这个函数是在覆盖基类虚函数”。

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

# 继承与组合

接手现代项目时，经常会看到“组合优于继承”的建议。原因不是继承不好，而是它容易导致：

- 层次过深
- 基类职责膨胀
- 派生类过度依赖基类实现细节
- 修改一个基类影响大量子类

如果两个类只是“使用关系”而非真正的“是一种”，优先考虑组合。

# 综合示例

```cpp
#include <iostream>
#include <string>

class Animal {
public:
    Animal(const std::string& name) : name_(name) {
    }

    virtual void speak() const {
        std::cout << "animal" << std::endl;
    }

protected:
    std::string name_;
};

class Dog : public Animal {
public:
    Dog(const std::string& name) : Animal(name) {
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

# 推荐实践

- 表达“is-a” 关系时才使用 `public` 继承。
- 只要是在重写虚函数，就加 `override`。
- 确认类型不应该再被继承时，考虑 `final`。
- 不要为了复用几行代码而引入继承层级。
- 优先考虑组合，再考虑继承。

# 小结

继承在 `C++11` 里依然重要，但现代代码更强调“谨慎使用”和“明确表达”。继承不是默认复用手段，而是一种带有强语义承诺的设计关系。

## 为什么现代 C++ 更克制地使用继承

很多早期面向对象资料会把继承当作主要复用方式，但现代 `C++` 更强调：

- 继承要表达稳定的 is-a 关系
- 组合通常比继承更容易维护
- 继承层级越深，生命周期和接口边界越难推理

所以“能不能继承”不是第一问题，“这是不是一个稳定的继承关系”才更关键。

## 先问是不是 is-a

如果一个类型只是“用到了另一个类型的能力”，通常更适合组合。

例如：

- `UserService` 有一个 `Logger`
- `OrderProcessor` 有一个 `Storage`

这种关系通常不是继承。

## 继承体系里特别值得关注的点

读代码时建议优先看：

- 基类是否设计为多态基类
- 基类析构是否为虚函数
- 派生类是否使用 `override`
- 是否有过深继承链

很多维护困难都来自这里。

## 常见误区

### 误区 1：继承是默认复用手段

不对。现代实践更倾向组合优先。

### 误区 2：只要语法能继承，就说明设计合理

不对。语法允许不等于语义稳定。

### 误区 3：`override` 只是可选装饰

不建议这么看。它是重要的接口校验工具。

## 补充建议

- 真正满足 is-a 关系时再使用 `public` 继承。
- 有虚函数的继承体系里优先检查虚析构。
- 实现覆盖时始终加 `override`。
- 没有明确抽象边界时，先考虑组合。
