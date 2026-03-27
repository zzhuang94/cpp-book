# 常量

`const` 在 `C++98` 时代就已经非常重要，而在 `C++11` 中，常量表达能力进一步增强，尤其是 `constexpr` 的加入，让“编译期常量”和“只读对象”不再混为一谈。

## `const` 的基本含义

`const` 表示对象初始化后不可修改：

```cpp
const int maxCount = 100;
// maxCount = 200; // 编译错误
```

这类常量适用于：

- 配置值
- 只读参数
- 不希望被误改的局部变量

## 常量引用

`const` 最常见的工程用途之一，是作为函数参数避免复制并保证只读：

```cpp
#include <iostream>
#include <string>

void printName(const std::string& name) {
    std::cout << name << std::endl;
}
```

这是旧时代 `C++` 就有的高频写法，到 `C++11` 仍然是核心习惯。

## 指针与 `const`

这部分很容易混淆：

```cpp
const int* p1 = nullptr;   // 不能通过 p1 修改所指向的数据
int* const p2 = nullptr;   // p2 本身不可改指向
const int* const p3 = nullptr; // 两者都不可改
```

记忆方法：看 `const` 修饰谁。

## `constexpr`

`C++11` 新增 `constexpr`，它比 `const` 更强调“编译期可求值”。

```cpp
constexpr int bufferSize = 256;
```

和 `const` 的区别可以先这样理解：

- `const`：运行期初始化后不可改
- `constexpr`：要求能在编译期求值

## `constexpr` 函数

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

## `const` 成员函数

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

## `const` 与 `auto`

`auto` 会进行类型推导，但 `const` 语义仍然需要注意：

```cpp
const int x = 10;
auto a = x;        // int
const auto b = x;  // const int
```

如果你依赖只读语义，不要假设 `auto` 会自动替你保留所有限定信息。

## 一个综合示例

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

## 推荐实践

- 能只读就加 `const`，把接口意图表达清楚。
- 函数参数中，大对象优先使用 `const T&`。
- 真正的编译期常量优先考虑 `constexpr`。
- 为不会修改状态的成员函数加上 `const`。
- 不要把 `const` 仅仅当成“防止误修改”的修饰词，它也是接口设计的一部分。

## 小结

`const` 解决的是“不可修改”，`constexpr` 解决的是“能否在编译期求值”。理解这两者的区别，是从老式 `C++` 向 `C++11` 过渡时非常关键的一步。
