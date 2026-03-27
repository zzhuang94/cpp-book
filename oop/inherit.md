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
