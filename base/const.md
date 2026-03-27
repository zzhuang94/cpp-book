# 常量

`const` 在 `C++98` 时代就已经非常重要，而在 `C++11` 中，常量表达能力进一步增强，尤其是 `constexpr` 的加入，让“编译期常量”和“只读对象”不再混为一谈。

# `const`

`const` 表示对象初始化后不可修改：

```cpp
const int maxCount = 100;
// maxCount = 200; // 编译错误
```

这类常量适用于：

- 配置值
- 只读参数
- 不希望被误改的局部变量

# 常量引用

`const` 最常见的工程用途之一，是作为函数参数避免复制并保证只读：

```cpp
#include <iostream>
#include <string>

void printName(const std::string& name) {
    std::cout << name << std::endl;
}
```

这是旧时代 `C++` 就有的高频写法，到 `C++11` 仍然是核心习惯。

# 指针与 `const`

这部分很容易混淆：

```cpp
const int* p1 = nullptr;   // 不能通过 p1 修改所指向的数据
int* const p2 = nullptr;   // p2 本身不可改指向
const int* const p3 = nullptr; // 两者都不可改
```

记忆方法：看 `const` 修饰谁。

# `constexpr`

`C++11` 新增 `constexpr`，它比 `const` 更强调“编译期可求值”。

```cpp
constexpr int bufferSize = 256;
```

和 `const` 的区别可以先这样理解：

- `const`：运行期初始化后不可改
- `constexpr`：要求能在编译期求值

# `constexpr` 函数

`C++11` 允许定义简单的 `constexpr` 函数：

```cpp
constexpr int square(int x) {
    return x * x;
}
```

这样就可以在需要编译期常量的地方使用：

```cpp
int arr[square(4)];
```

# `const` 成员函数

成员函数后面的 `const` 表示不会修改对象状态：

```cpp
class User {
public:
    User(const std::string& name) : name_(name) {}

    const std::string& name() const {
        return name_;
    }

private:
    std::string name_;
};
```

这是类接口设计的重要部分，和“这个对象是否可读”直接相关。

# `const` 与 `auto`

`auto` 会进行类型推导，但 `const` 语义仍然需要注意：

```cpp
const int x = 10;
auto a = x;        // int
const auto b = x;  // const int
```

如果你依赖只读语义，不要假设 `auto` 会自动替你保留所有限定信息。

# 综合示例

```cpp
#include <iostream>
#include <string>

constexpr int maxLength() {
    return 64;
}

void printMessage(const std::string& message) {
    std::cout << message << std::endl;
}

int main() {
    const std::string title = "C++11";
    char buffer[maxLength()] = {};

    printMessage(title);
    std::cout << "buffer size = " << sizeof(buffer) << std::endl;
    return 0;
}
```

# 推荐实践

- 能只读就加 `const`，把接口意图表达清楚。
- 函数参数中，大对象优先使用 `const T&`。
- 真正的编译期常量优先考虑 `constexpr`。
- 为不会修改状态的成员函数加上 `const`。
- 不要把 `const` 仅仅当成“防止误修改”的修饰词，它也是接口设计的一部分。

# 小结

`const` 解决的是“不可修改”，`constexpr` 解决的是“能否在编译期求值”。理解这两者的区别，是从老式 `C++` 向 `C++11` 过渡时非常关键的一步。

## 为什么 `const` 在现代 C++ 里几乎是主线能力

很多其它语言开发者会把 `const` 理解成“一个小修饰词”，但在 `C++` 里，它常常承担更大的角色：

- 描述接口意图
- 帮助编译器限制错误调用
- 改善代码可读性
- 让对象是否可变这件事变得更明确

你在真实项目里看到的很多高质量 `C++` 代码，通常都会大量使用 `const`。

## `const` 是接口设计，而不只是防手误

例如：

```cpp
void printUser(const std::string& name);
```

这里的 `const` 不只是“我怕你改参数”，而是在向调用者明确表达：

- 这个函数只读这个参数
- 传大对象时不会多做一份复制

这种信息对于阅读 API 很重要。

## `const` 成员函数让对象读写边界更清楚

当你看到：

```cpp
const std::string& name() const;
```

你至少能快速知道两件事：

- 这是个读接口
- 调用它不会修改对象的逻辑状态

如果一个类把“哪些函数会修改对象、哪些不会”表达得很清楚，后续维护成本会低很多。

## `constexpr` 不要理解成“更高级的 const”

更准确的第一印象应该是：

- `const` 强调只读
- `constexpr` 强调编译期可求值

有时两者会重合，但关注点并不完全一样。对初学者来说，先把它们区分开已经非常重要。

## `auto` 和 `const` 一起使用时别掉以轻心

例如：

```cpp
const std::string name = "cpp";
auto a = name;
const auto b = name;
```

这里：

- `a` 是普通值拷贝
- `b` 才保留只读语义

这提醒我们：类型推导不是魔法，推导出来的结果仍然要靠你主动检查和理解。

## `mutable` 是 `const` 的一个例外入口

虽然本章不是 `mutable` 专题，但需要先知道它的存在：某些成员即使在 `const` 成员函数里也允许修改，常见于缓存、命中计数等“逻辑常量但实现上可变”的场景。

这能帮助你读懂一些看起来“明明是 const，为什么还能改成员”的代码。

## 常见误区

### 误区 1：`const` 只是锦上添花，可加可不加

不对。很多时候它本身就是接口信息的一部分。

### 误区 2：`constexpr` 就是“更强的 const”

不完全对。它的核心强调点是编译期求值能力。

### 误区 3：用了 `auto` 就会自动保留所有只读限定

不对。推导规则需要你自己理解。

## 补充建议

- 能只读的局部变量就尽量加 `const`。
- 大对象只读参数优先 `const T&`。
- 成员函数能不改状态就加 `const`。
- 真正用于编译期常量表达的值优先考虑 `constexpr`。
- 看到复杂声明时，先分清 `const` 修饰的是对象、指针还是所指向的数据。
