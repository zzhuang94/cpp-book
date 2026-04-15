# 常量

- `const` 在 `C++98` 时代就已经非常重要
- 而在 `C++11` 中，常量表达能力进一步增强，尤其是 `constexpr` 的加入，让“编译期常量”和“只读对象”不再混为一谈
- 这一章主要处理两件事：`只读边界` 和 `编译期边界`

----

# const

`const` 表示对象初始化后不可修改：

```cpp
const int maxCount = 100;
// maxCount = 200; // 编译错误
```

这类常量适用于：

- 配置值
- 只读参数
- 不希望被误改的局部变量

----

# 常量引用

`const T&` 是 `const` 最常见的工程用途之一。
它表达的是：

- 我只是借用对象
- 我不打算修改它
- 我想避免不必要复制

```cpp
#include <iostream>
#include <string>

void printName(const std::string& name) {
    std::cout << name << std::endl;
}
```

这里先建立“只读借用”的直觉，不把规则展开过深。
更完整的参数设计和引用语义，放在后面的 `引用` 一章里。

----

# const 与指针

`const` 和指针组合时，核心判断仍然是：

> `const` 修饰谁，谁就不能改。

这里只先建立阅读直觉，不把它当成这一章的主线展开。
更完整的指针语义和所有权讨论，放在后面的 `指针` 一章里。

```cpp
const int* p1 = nullptr;   // 不能通过 p1 修改所指向的数据
int* const p2 = nullptr;   // p2 本身不可改指向
const int* const p3 = nullptr; // 两者都不可改
```

----

# constexpr

`C++11` 新增 `constexpr`，它比 `const` 更强调“编译期可求值”。

```cpp
constexpr int bufferSize = 256;
```

和 `const` 的区别可以先这样理解：

- `const`：运行期初始化后不可改
- `constexpr`：要求能在编译期求值

----

# constexpr 函数

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

----

# const 成员函数

成员函数后面的 `const` 表示不会修改对象状态：

```cpp
class User {
public:
    // const 修饰参数引用指向的数据，表示不能通过 name 修改传入的字符串内容
    // 同时这里配合 &，避免按值传参带来的拷贝
    User(const std::string& name) : name_(name) {}

    // 第一个 const 修饰返回值引用指向的数据，表示调用者拿到的是“只读引用”
    // 也就是可以读这个字符串，但不能通过返回值去修改 name_
    const std::string& name() const {
        // 第二个 const 修饰成员函数本身，表示这是 const 成员函数
        // 在这个函数里，this 的类型相当于 const User*，因此不能修改对象的普通成员                 
        return name_;
    }

private:
    std::string name_;
};
```

这是类接口设计的重要部分，和“这个对象是否可读”直接相关。
所以这一节仍然保留在基础层，因为它直接影响你阅读类接口。

----

# const 与 auto

`auto` 会进行类型推导，但 `const` 语义仍然需要注意：

```cpp
const int x = 10;
auto a = x;        // int
const auto b = x;  // const int
```

如果你依赖只读语义，不要假设 `auto` 会自动替你保留所有限定信息。

----

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

- 读到 `const` 和 `constexpr` 时，最好不要只把它们当成“不能改”的语法标签。
- 它们更像是在给代码补充边界信息。
- 这个值是不是只读。
- 这个结果能不能在编译期确定。
- 这个接口会不会修改对象状态。

----

# 推荐实践

- 能只读就加 `const`，把接口意图表达清楚。
- 只读借用参数时，经常会用 `const T&`，细节放到 `引用` 一章再展开。
- 真正的编译期常量优先考虑 `constexpr`。
- 为不会修改状态的成员函数加上 `const`。
- 不要把 `const` 仅仅当成“防止误修改”的修饰词，它也是接口设计的一部分。

----

# 小结

- `const` 解决的是只读边界，`constexpr` 解决的是编译期求值边界。
- 把这两件事分清楚之后，你读接口、读类定义和写常量表达式时都会更稳，也更容易看懂现代 `C++` 代码为什么这样写。
