# 字符串

`C++98` 时代，很多项目还在 `char*`、字符数组和 `std::string` 之间混用。到了 `C++11`，虽然底层字符模型没有被彻底重做，但 `std::string` 的地位已经非常明确：业务代码里优先把它当成默认字符串类型。

# `std::string`

最常见的字符串类型来自标准库：

```cpp
#include <iostream>
#include <string>

int main() {
    std::string name = "C++11";
    std::cout << name << std::endl;
    return 0;
}
```

相比 C 风格字符串，它的优势是：

- 自动管理内存
- 支持长度查询
- 支持拼接、比较、查找
- 与标准库容器和算法协作更自然

# 基本操作

```cpp
#include <iostream>
#include <string>

int main() {
    std::string first = "Hello";
    std::string second = "World";
    std::string text = first + ", " + second;

    std::cout << text << std::endl;
    std::cout << text.size() << std::endl;
    return 0;
}
```

# 访问字符

```cpp
#include <iostream>
#include <string>

int main() {
    std::string text = "hello";
    std::cout << text[0] << std::endl;
    std::cout << text.at(1) << std::endl;
    return 0;
}
```

区别：

- `operator[]` 不做边界检查
- `at()` 会在越界时抛出异常

# 查找与截取

```cpp
#include <iostream>
#include <string>

int main() {
    std::string text = "hello cpp11";
    auto pos = text.find("cpp");

    if (pos != std::string::npos) {
        std::cout << text.substr(pos, 5) << std::endl;
    }

    return 0;
}
```

# C 风格字符串

旧代码中非常常见：

```cpp
char name[] = "hello";
const char* p = "world";
```

这类写法今天仍然存在，但更适合：

- 与 C 接口交互
- 处理底层字符缓冲区
- 性能要求特殊的场景

业务层代码若不是被接口限制，优先使用 `std::string`。

# `c_str()`

当你必须和 C API 交互时，可以从 `std::string` 获取 `const char*`：

```cpp
#include <cstdio>
#include <string>

int main() {
    std::string text = "hello";
    std::printf("%s\n", text.c_str());
    return 0;
}
```

# 原始字符串

`C++11` 新增原始字符串字面量，非常适合包含转义字符或多行内容的文本。

```cpp
#include <iostream>
#include <string>

int main() {
    std::string json = R"({"name":"cpp11","version":11})";
    std::cout << json << std::endl;
    return 0;
}
```

# Unicode 字符类型

`C++11` 引入：

- `char16_t`
- `char32_t`

它们让字符宽度表达更明确，但完整的 Unicode 文本处理远比“换一个字符类型”复杂。大多数项目里，这部分通常会依赖专门库或平台约定。

# 综合示例

```cpp
#include <iostream>
#include <string>

int main() {
    std::string language = "C++";
    std::string version = "11";
    std::string full = language + version;

    if (full.find("C++") != std::string::npos) {
        std::cout << full << std::endl;
    }

    for (char ch : full) {
        std::cout << ch << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

# 推荐实践

- 业务代码优先使用 `std::string`。
- 需要与 C API 交互时再使用 `c_str()`。
- 读取字符时若担心越界，优先 `at()`。
- 复杂文本常量优先考虑原始字符串字面量。
- 不要把 `char*` 当成通用字符串类型继续滥用。

# 小结

字符串处理是现代 `C++` 代码风格差异最明显的地方之一。你从 `char*` 迁移到 `std::string`，本质上是在把“手动管理字符缓冲区”升级为“使用类型安全、可组合的标准库对象”。

## 对其它语言开发者来说，这章真正要建立什么

如果你平时写 `Go`、`PHP`、`JS`、`TS`，字符串往往默认就是高层对象；而进入 `C++` 后，你会同时遇到：

- `std::string`
- `char*`
- `const char*`
- 字符数组

所以这章真正要建立的不是“字符串能拼接”，而是：

- 业务代码默认优先 `std::string`
- `const char*` 多数时候只是与 C 风格接口交互的桥梁
- 字符串对象本身也有生命周期和拷贝成本

## `std::string` 是默认选择，但不是“魔法字符串”

`std::string` 帮你管理内存和长度，这是巨大的进步；但它仍然是一个对象：

- 按值传递可能复制
- 返回引用要考虑生命周期
- `c_str()` 指针的有效性依赖原字符串对象是否仍然存在

这和托管语言里的“字符串总是安全抽象”还是有差别的。

## `c_str()` 最值得记住的事

`c_str()` 很常用，但你至少要记住一件事：

> 返回的 `const char*` 依附于原始 `std::string` 对象。

只要字符串对象被销毁或内容被改动，这个指针就可能不再可靠。因此不要轻易长期保存它。

## `operator[]` 和 `at()` 的差别要有边界意识

如果你只是想快速访问且已经确认索引安全，`operator[]` 很方便；但如果处于调试、学习或边界不够确定的场景，`at()` 的异常行为会更容易帮你定位问题。

这体现的是现代 `C++` 很典型的一种思路：在性能和安全之间，要根据上下文选择更合适的接口，而不是默认什么都裸来。

## 常见误区

### 误区 1：`char*` 和 `std::string` 差不多

不对。前者通常只是裸字符指针，后者是拥有内容的标准库对象。

### 误区 2：拿到 `c_str()` 之后就可以一直保存

不对。它依赖原字符串对象的生命周期。

### 误区 3：字符串章节只是 API 记忆题

不对。它同时牵涉对象、边界检查、参数传递和 C API 交互。

## 补充建议

- 业务代码默认优先 `std::string`。
- 大字符串只读参数优先 `const std::string&`。
- 只有与 C 接口交互时才转成 `c_str()`。
- 边界不确定时优先 `at()`。
- 看到 `const char*` 时先判断它是字面量、借用视图还是底层接口桥接。
