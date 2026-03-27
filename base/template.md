# 模板入门

很多其它语言开发者第一次接触 `C++` 模板时，都会有一点心理压力，因为模板在历史上确实以“难学、报错长、语法绕”著称。

但从项目上手角度看，模板并不是一个可以完全跳过的高级话题。原因很简单：

- 标准库大量容器和算法本身就是模板
- 你每天都在使用 `std::vector<int>`、`std::map<std::string, int>` 这种模板实例
- 现代 `C++` 的类型安全和泛型能力，很大一部分都建立在模板上

所以这一章的目标不是把模板讲成元编程大全，而是先让你看懂最常见的模板代码。

## 可以先把模板理解成什么

先用一句不严格但很实用的话理解：

> 模板是 `C++` 在编译期生成“针对不同类型的代码”的机制。

它和其它语言里的泛型有相似之处，但不完全一样。最关键的区别之一是：

- 很多语言的泛型更偏“统一规则 + 运行时或编译器约束”
- `C++` 模板更偏“按你提供的类型生成具体代码”

## 函数模板

最常见的入门形式是函数模板：

```cpp
#include <iostream>

template <typename T>
T maxValue(T a, T b) {
    return (a > b) ? a : b;
}

int main() {
    std::cout << maxValue(3, 7) << std::endl;
    std::cout << maxValue(2.5, 1.2) << std::endl;
    return 0;
}
```

这里的意思是：

- `T` 是一个类型参数
- 编译器会根据调用时的实参类型推导 `T`

## `typename` 和 `class`

在模板参数列表里，这两种写法基本等价：

```cpp
template <typename T>
```

```cpp
template <class T>
```

现代代码里很多人更喜欢 `typename`，因为它更直接表达“这里是一个类型参数”。

## 模板参数推导

看下面代码：

```cpp
maxValue(1, 2);
```

编译器会推导出 `T` 是 `int`。

```cpp
maxValue(1.5, 2.5);
```

会推导出 `T` 是 `double`。

但如果你写：

```cpp
maxValue(1, 2.5);
```

就可能推导失败或产生你不想要的转换。因此模板虽然很灵活，仍然需要你对类型匹配保持敏感。

## 类模板

除了函数模板，还有类模板：

```cpp
#include <iostream>

template <typename T>
class Box {
public:
    explicit Box(const T& value) : value_(value) {
    }

    const T& get() const {
        return value_;
    }

private:
    T value_;
};

int main() {
    Box<int> a(10);
    Box<std::string> b("cpp11");

    std::cout << a.get() << std::endl;
    std::cout << b.get() << std::endl;
    return 0;
}
```

这里的 `Box<int>`、`Box<std::string>` 就是不同的模板实例。

## 为什么标准库容器都是模板

例如：

```cpp
std::vector<int>
std::vector<std::string>
std::map<std::string, int>
```

容器本身的行为规则相似，但存储元素类型不同。模板正好适合这种场景：

- 规则统一
- 类型变化
- 希望保留强类型和高性能

## 模板和 `auto`

在现代 `C++` 里，模板和 `auto` 经常配合出现。因为模板展开后的类型可能很长，而 `auto` 可以帮你减少噪音。

例如：

```cpp
std::map<std::string, int> scores;
auto it = scores.begin();
```

如果不写 `auto`，迭代器类型会更冗长。

## 模板与内联实现

很多时候你会发现模板类或模板函数的实现直接写在头文件里。初学者容易奇怪：不是说头文件放声明、源文件放定义吗？

模板是一个重要例外。原因可以先粗略理解为：

> 编译器在实例化模板时，通常需要看到完整定义。

所以模板代码经常放在头文件里，这和普通非模板函数的组织方式不完全一样。

## 一个标准库模板的直觉例子

看这段代码：

```cpp
#include <iostream>
#include <vector>

template <typename T>
void printAll(const std::vector<T>& values) {
    for (const auto& value : values) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
}
```

这里的模板函数可以打印不同类型的 `vector`，只要里面的元素支持输出运算符。

这也引出模板的一个重要特征：

> 模板很多时候不是要求某个类型“继承了某个基类”，而是要求“这个类型支持某些操作”。

## 与其它语言泛型的差异

如果你来自 `Go` / `TS` 等语言，可以先记住几个差异：

- `C++` 模板更早、更底层，也更自由。
- 它不是统一运行时泛型模型，而更像编译期代码生成。
- 模板约束在 `C++11` 时代不像更新标准那样直观，经常通过报错信息间接体现。
- 许多错误会在实例化时暴露，而不是模板定义时就全部明确。

这也是为什么模板报错信息 historically 比较长。

## 常见的第二个模板参数

你会经常看到不止一个模板参数：

```cpp
template <typename Key, typename Value>
class PairStorage {
};
```

标准库容器中这类形式非常常见，例如 `std::map<Key, Value>`。

## 模板与引用

模板遇到引用时，行为会更复杂。例如：

```cpp
template <typename T>
void print(const T& value) {
    std::cout << value << std::endl;
}
```

这里用 `const T&` 可以：

- 避免大对象复制
- 接受多种类型
- 绑定临时对象

这也是很多通用工具函数的常见写法。

## 一个简单的类模板示例

```cpp
#include <iostream>
#include <string>

template <typename T>
class Holder {
public:
    Holder() = default;

    explicit Holder(const T& value) : value_(value) {
    }

    void set(const T& value) {
        value_ = value;
    }

    const T& get() const {
        return value_;
    }

private:
    T value_{};
};

int main() {
    Holder<int> number(42);
    Holder<std::string> text("hello");

    std::cout << number.get() << std::endl;
    std::cout << text.get() << std::endl;
    return 0;
}
```

这个例子虽然简单，但已经足够说明模板如何帮助你写出“规则相同、类型不同”的组件。

## 模板读代码技巧

当你读到模板代码时，建议先按这个顺序看：

1. 先看模板参数有哪些。
2. 再看这个模板对类型提出了什么隐含要求。
3. 再看它是按值、按引用还是按常量引用接收参数。
4. 最后再看实例化时具体传入了什么类型。

很多时候你并不需要一开始就理解每个尖括号里的细节，只要先搞清楚“这个组件想对不同类型做同一类事情”即可。

## 常见误区

### 误区 1：模板只是高级技巧，平时用不到

不对。你每天使用的标准库容器几乎都建立在模板上。

### 误区 2：模板等于宏

不对。模板有类型系统参与，和纯文本替换的宏完全不是一回事。

### 误区 3：模板一定意味着复杂元编程

不对。很多项目里最常见的模板只是容器、简单工具函数和轻量通用组件。

### 误区 4：模板和面向对象是完全替代关系

不对。它们解决的问题不同，模板偏泛型复用，多态偏运行时抽象。

## 给初学者的实践建议

- 先把函数模板和类模板看懂，再去碰更复杂的模板技巧。
- 读标准库代码时，先识别模板参数和实例类型，不要被尖括号吓住。
- 在 `C++11` 阶段，先掌握“如何使用模板”，不必急着深入模板元编程。
- 模板实现常放头文件，这是正常现象。
- 报错看不懂时，先从最靠近自己调用点的那一层开始看。

## 小结

模板是 `C++` 泛型能力的核心，也是标准库大量能力的基础。对初学者来说，先把模板理解为“编译期按类型生成具体代码的机制”就已经足够实用。只要你能看懂函数模板、类模板以及 `std::vector<T>` 这类常见实例，阅读现代 `C++11` 项目代码的门槛就会大幅下降。
