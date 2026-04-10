# 模板入门

很多其它语言开发者第一次接触 `C++` 模板时，都会有一点心理压力，因为模板在历史上确实以"难学、报错长、语法绕"著称。

但从项目上手角度看，模板并不是一个可以完全跳过的高级话题。原因很简单：

- 标准库大量容器和算法本身就是模板
- 你每天都在使用 `std::vector<int>`、`std::map<std::string, int>` 这种模板实例
- 现代 `C++` 的类型安全和泛型能力，很大一部分都建立在模板上

所以这一章的目标不是把模板讲成元编程大全，而是先让你看懂最常见的模板代码。

## 可以先把模板理解成什么

先用一句不严格但很实用的话理解：

> 模板是 `C++` 在编译期生成"针对不同类型的代码"的机制。

它和其它语言里的泛型有相似之处，但不完全一样。最关键的区别之一是：

- 很多语言的泛型更偏"统一规则 + 运行时或编译器约束"
- `C++` 模板更偏"按你提供的类型生成具体代码"

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

现代代码里很多人更喜欢 `typename`，因为它更直接表达"这里是一个类型参数"。

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

> 模板很多时候不是要求某个类型"继承了某个基类"，而是要求"这个类型支持某些操作"。

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

这个例子虽然简单，但已经足够说明模板如何帮助你写出"规则相同、类型不同"的组件。

## 模板读代码技巧

当你读到模板代码时，建议先按这个顺序看：

1. 先看模板参数有哪些。
2. 再看这个模板对类型提出了什么隐含要求。
3. 再看它是按值、按引用还是按常量引用接收参数。
4. 最后再看实例化时具体传入了什么类型。

很多时候你并不需要一开始就理解每个尖括号里的细节，只要先搞清楚"这个组件想对不同类型做同一类事情"即可。

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
- 在 `C++11` 阶段，先掌握"如何使用模板"，不必急着深入模板元编程。
- 模板实现常放头文件，这是正常现象。
- 报错看不懂时，先从最靠近自己调用点的那一层开始看。

----

# 进阶用法

上面介绍的基础模板已经够你读懂大多数日常代码了。但当你开始阅读像线程池、网络库这类基础设施代码时，会碰到一些更高级的模板特性。

这一节不追求面面俱到，只覆盖实际项目中**最常遇到**的几个。

## `decltype`：编译期类型推导

`auto` 让你在定义变量时省略类型，而 `decltype` 让你在**不实际执行表达式**的情况下拿到它的类型：

```cpp
int x = 42;
decltype(x) y = 10;  // y 的类型是 int，因为 x 是 int
```

更常见的用法是推导函数调用的返回类型：

```cpp
int add(int a, int b) { return a + b; }

decltype(add(1, 2)) result = add(3, 4);  // result 的类型是 int
```

`decltype` 不会真的调用 `add(1, 2)`，它只是让编译器在编译期推导出"如果调用 `add(1, 2)`，返回类型是什么"。

## 尾置返回类型

普通函数的返回类型写在前面：

```cpp
int add(int a, int b) { return a + b; }
```

但如果返回类型需要依赖参数，写在前面时参数还没声明，编译器看不到。尾置返回类型用 `auto` + `->` 把返回类型放到参数列表之后：

```cpp
auto add(int a, int b) -> int {
    return a + b;
}
```

上面这个例子没什么意义，但结合模板和 `decltype` 就非常有用了：

```cpp
template <typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}
```

这里 `decltype(a + b)` 让编译器自动推导出 `int + double` 返回 `double`、`string + string` 返回 `string` 等等。如果把返回类型写在前面，`a` 和 `b` 还没出现，`decltype` 无法使用。

## 变参模板

基础模板只有固定个数的类型参数，而变参模板可以接受**任意个数**的类型参数：

```cpp
// typename... Args 表示"零个或多个类型参数"，称为参数包（parameter pack）
template <typename... Args>
void print(Args... args);
```

配合递归可以逐个处理参数：

```cpp
#include <iostream>

// 递归终止条件：只剩一个参数
template <typename T>
void print(T value) {
    std::cout << value << std::endl;
}

// 递归展开：取出第一个参数，剩余的继续递归
template <typename T, typename... Rest>
void print(T first, Rest... rest) {
    std::cout << first << " ";
    print(rest...);  // rest... 展开剩余参数
}

int main() {
    print(1, "hello", 3.14, 'x');
    // 输出: 1 hello 3.14 x
    return 0;
}
```

在实际项目中，变参模板最常见的用法是让一个函数**透传任意参数**给另一个函数，而不需要关心参数的个数和类型。后面的完美转发会展示这一点。

## 万能引用与完美转发

### 万能引用

在模板中，`T&&` 不是普通的右值引用，而是**万能引用**（universal reference），它既能绑定左值也能绑定右值：

```cpp
template <typename T>
void wrapper(T&& arg) {
    // 如果传入左值，T 推导为 Type&，arg 是左值引用
    // 如果传入右值，T 推导为 Type，arg 是右值引用
}

int x = 10;
wrapper(x);    // T = int&,  arg 是 int&（左值引用）
wrapper(42);   // T = int,   arg 是 int&&（右值引用）
```

> 注意：只有在**模板参数推导**场景下 `T&&` 才是万能引用。普通函数中的 `int&&` 就是纯右值引用。

### `std::forward`

万能引用有个问题：一旦参数有了名字，它在函数体内就是左值，原始的"左值/右值"信息会丢失。`std::forward` 可以恢复这个信息：

```cpp
#include <utility>  // std::forward
#include <iostream>

void process(int& x)  { std::cout << "左值: " << x << std::endl; }
void process(int&& x) { std::cout << "右值: " << x << std::endl; }

template <typename T>
void wrapper(T&& arg) {
    // 不用 forward：arg 有名字，永远按左值传递
    // 用 forward：保持原始的左值/右值属性
    process(std::forward<T>(arg));
}

int main() {
    int x = 10;
    wrapper(x);    // 输出: 左值: 10
    wrapper(42);   // 输出: 右值: 42
    return 0;
}
```

### 组合起来

在实际代码中，变参模板 + 万能引用 + `std::forward` 经常一起出现，用来**透传任意参数给内部函数，且不产生额外拷贝**：

```cpp
template <typename F, typename... Args>
auto submit(F&& f, Args&&... args) {
    // std::forward<F>(f)         保持 f 的左值/右值属性
    // std::forward<Args>(args)... 逐个保持每个参数的左值/右值属性
    return f(std::forward<Args>(args)...);
}
```

如果你读到 [高并发](advanced/concurrency.md) 章节的线程池代码，会看到 `submit` 方法正是这个模式：

```cpp
template <typename F, typename... Args>
auto submit(F&& f, Args&&... args)
    -> std::future<decltype(f(args...))>
```

它组合了本节介绍的所有特性：变参模板接受任意参数、万能引用避免拷贝、`decltype` 推导返回类型、尾置返回类型解决声明顺序问题。

## `std::bind`

`std::bind` 可以把一个函数和它的部分或全部参数绑定在一起，生成一个新的可调用对象：

```cpp
#include <functional>
#include <iostream>

int add(int a, int b) {
    return a + b;
}

int main() {
    // 把 add 的第一个参数固定为 10，生成一个只需要一个参数的新函数
    auto add10 = std::bind(add, 10, std::placeholders::_1);
    std::cout << add10(5) << std::endl;  // 输出: 15

    // 绑定所有参数，生成一个无参函数
    auto add_3_4 = std::bind(add, 3, 4);
    std::cout << add_3_4() << std::endl; // 输出: 7
    return 0;
}
```

在线程池中，`std::bind` 用来把用户传入的函数和参数绑定成一个**无参可调用对象**，这样工作线程只需要调用 `task()` 就行，不需要知道原始函数的签名。

## 读懂组合模板代码的方法

当你遇到像线程池 `submit` 那样把多个特性堆在一起的代码时，建议按以下顺序拆解：

1. **先看模板参数**：`F` 是什么？`Args...` 是什么？
2. **再看返回类型**：`decltype(...)` 推导的是哪个表达式？
3. **再看 `std::forward`**：哪些参数被转发了？转发到哪里？
4. **最后看整体流程**：输入是什么，经过什么变换，输出是什么？

不必试图一次性看懂所有尖括号，逐层拆解就好。

## 小结

模板是 `C++` 泛型能力的核心，也是标准库大量能力的基础。对初学者来说，先把模板理解为"编译期按类型生成具体代码的机制"就已经足够实用。只要你能看懂函数模板、类模板以及 `std::vector<T>` 这类常见实例，阅读现代 `C++11` 项目代码的门槛就会大幅下降。

进阶用法中的变参模板、完美转发、`decltype` 等特性，在阅读基础设施代码（线程池、网络库等）时会频繁出现。掌握它们的核心思路——"透传任意参数、不丢失类型信息、不产生额外拷贝"——就足以应对大多数场景。
