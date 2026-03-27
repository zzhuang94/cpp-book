# 多态

多态是 `C++` 面向对象最核心的能力之一。你以前可能已经学过“基类指针指向派生类对象、调用虚函数”的基本套路，但在现代 `C++` 里，真正重要的是：如何安全地写虚函数接口，如何避免对象切片，以及为什么 `override` 和虚析构函数如此关键。

## 什么是多态

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

## 通过基类引用或指针实现多态

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

这里如果没有 `virtual`，调用的就是静态绑定版本。

## 虚析构函数

只要类打算作为基类并通过基类指针删除派生对象，就应提供虚析构函数：

```cpp
class Base {
public:
    virtual ~Base() = default;
};
```

否则可能导致只调用基类析构，派生类资源无法正确释放。

## `override`

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

没有 `override` 时，这很可能只是新增了一个函数；加了 `override`，编译器会立即告诉你签名不匹配。

## 对象切片

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

如果把 `Dog` 以值传递给 `test()`，派生类部分会被切掉，多态失效。因此多态接口通常使用：

- `Animal&`
- `const Animal&`
- `Animal*`
- 智能指针

## 动态绑定的成本

虚函数通常通过虚函数表实现，会带来：

- 一次间接调用
- 对象布局中额外的虚表指针

大多数业务代码里，这个成本通常可以接受。不要在没有证据的情况下为了“极致性能”回避合理的多态设计。

## 智能指针与多态

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
    std::unique_ptr<Animal> animal(new Dog());
    animal->speak();
    return 0;
}
```

这比手动 `new/delete` 更安全。

## 一个综合示例

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
    shapes.push_back(std::unique_ptr<Shape>(new Circle()));
    shapes.push_back(std::unique_ptr<Shape>(new Rectangle()));

    for (const auto& shape : shapes) {
        shape->draw();
    }

    return 0;
}
```

## 推荐实践

- 作为基类使用的类型通常应有虚析构函数。
- 重写虚函数时始终加 `override`。
- 多态接口优先通过引用、指针或智能指针传递。
- 注意对象切片，不要随手按值传递基类对象。
- 纯接口类优先只暴露必要虚函数。

## 小结

多态的核心不只是“虚函数能动态分派”，而是“如何在继承体系中安全、明确地表达扩展点”。`C++11` 的 `override` 和智能指针，让这件事比旧时代可靠了很多。
