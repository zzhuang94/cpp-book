# 引用

引用在旧版 `C++` 中就已经是核心特性，但到了 `C++11`，引用的世界被彻底扩展了。除了你熟悉的左值引用 `T&`，你还必须掌握右值引用 `T&&`，因为这直接关系到移动语义、性能优化和现代类设计。

# 左值引用

最基本的引用写法：

```cpp
#include <iostream>

int main() {
    int value = 10;
    int& ref = value;

    ref = 20;
    std::cout << value << std::endl;
    return 0;
}
```

引用本质上是对象的别名，不是重新拷贝一份。

# 常量引用

工程里最常见的引用形式之一：

```cpp
#include <iostream>
#include <string>

void print(const std::string& text) {
    std::cout << text << std::endl;
}
```

优点：

- 避免复制
- 保证只读
- 可绑定临时对象

# 左值与右值

理解 `C++11` 引用前，必须先区分：

- 左值：有持久身份、可取地址的对象
- 右值：临时对象、字面量或即将销毁的值

例如：

```cpp
int a = 10; // a 是左值
10;         // 10 是右值
```

# 右值引用

`C++11` 引入右值引用：

```cpp
int&& r = 10;
```

它的主要意义不是“多一种引用语法”，而是让语言能够区分：

- 这个对象是长期存在的
- 还是一个可以安全“偷走资源”的临时值

# 移动语义

看一个字符串例子：

```cpp
#include <iostream>
#include <string>

int main() {
    std::string a = "hello world";
    std::string b = std::move(a);

    std::cout << "b = " << b << std::endl;
    std::cout << "a = " << a << std::endl;
    return 0;
}
```

`std::move` 本身不移动任何东西，它只是把对象显式转换为右值，从而告诉编译器：这个对象的资源可以被移动。

# 右值引用价值

在 `C++98` 里，临时对象往往只能复制：

- 返回大对象时可能产生额外开销
- 容器扩容时可能复制元素
- 资源对象难以高效转移

`C++11` 有了右值引用后，可以把内部资源直接“搬走”，而不是复制一份。

# 引用折叠

这是更进阶的话题，但你至少要知道：

```cpp
template <typename T>
void f(T&& value) {
}
```

在模板推导里，这种 `T&&` 不一定总是右值引用，它可能形成所谓的“转发引用”或“万能引用”。这属于现代模板编程的重要基础。

# 综合示例

```cpp
#include <iostream>
#include <string>
#include <utility>

void print(const std::string& text) {
    std::cout << "const ref: " << text << std::endl;
}

void print(std::string&& text) {
    std::cout << "rvalue ref: " << text << std::endl;
}

int main() {
    std::string name = "cpp11";

    print(name);
    print("temporary");
    print(std::move(name));
    return 0;
}
```

这个例子展示了同一个接口如何根据左值/右值匹配不同重载。

# 推荐实践

- 只读大对象参数优先使用 `const T&`。
- 需要接管临时对象资源时使用 `T&&`。
- 只有确实要把对象当作右值处理时才调用 `std::move`。
- 被 `std::move` 过的对象仍然有效，但值处于未指定状态，不要假定其内容。
- 理解左值/右值，是理解移动语义和完美转发的前提。

# 小结

引用在 `C++11` 中已经从“避免复制的别名工具”升级为“性能语义和对象生命周期表达工具”。如果你能真正掌握左值引用、右值引用和 `std::move` 的关系，现代 `C++` 的很多设计都会突然变得顺理成章。
