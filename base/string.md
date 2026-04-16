# 字符串

- `C++98` 时代，很多项目还在 `char*`、字符数组和 `std::string` 之间混用。
- `C++11`虽然底层字符模型没有被彻底重做，但 `std::string` 的地位已经非常明确：**业务代码里优先把它当成默认字符串类型。**

----

# std::string

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

----

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

> 这里可以先建立一个很重要的认识：`std::string` 不是“会自动变长的字符数组”，它首先是一个标准库对象。

这意味着你在使用它时，不只是记几个成员函数，还要同时考虑：

- 它有自己的生命周期
- 按值传递可能发生拷贝
- 与 C 接口交互时，经常需要显式转换

----

# 访问字符

```cpp
#include <iostream>
#include <string>

int main() {
    std::string text = "hello";
    std::cout << text[0] << std::endl;      // h
    std::cout << text.at(1) << std::endl;   // e
    std::cout << text.size() << std::endl;  // 5
    std::cout << text[10] << std::endl;     // 越界，报错
    return 0;
}
```

- `operator[]` 不做边界检查
- `at()` 会在越界时抛出异常

----

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

----

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

> 业务层代码若不是被接口限制，优先使用 `std::string`。

## c_str()

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

要特别留意一件事：`c_str()` 返回的指针依附于原来的字符串对象。

- 只要原字符串还活着，这个指针通常就是可用的
- 一旦字符串对象销毁，这个指针就失效
- 如果字符串内容发生改变，也不应该继续假设旧指针仍然可靠

所以它更适合“临时把 `std::string` 借给 C 接口用一下”，而不是长期保存。

----

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

----

# Unicode 字符类型

`C++11` 引入：

- `char16_t`
- `char32_t`

它们让字符宽度表达更明确，但完整的 Unicode 文本处理远比“换一个字符类型”复杂。

> 大多数项目里，这部分通常会依赖专门库或平台约定。

----

# 作为函数参数

字符串在真实项目里最常见的使用场景之一，是作为函数参数传来传去。

```cpp
#include <iostream>
#include <string>

void printText(const std::string& text) {
    std::cout << text << std::endl;
}
int main() {
    std::string title = "C++11 string";
    printText(title);
    return 0;
}
```

这里常见的几种写法可以这样理解：

- `std::string text`：按值传递，函数拿到一个副本
- `const std::string& text`：只读引用，通常更适合较大的字符串
- `std::string& text`：可修改引用，表示函数会直接改原字符串

如果只是读取内容，优先考虑 `const std::string&`，这和 `func-base.md` 里讲参数语义的思路是一致的。

----

# 综合示例

```cpp
#include <iostream>
#include <string>

int main() {
    std::string language = "C++";
    std::string version = "11";
    std::string full = language + version;

    if (full.find("C++") != std::string::npos) {
        std::cout << "full: " << full << std::endl;
    }

    for (char ch : full) {
        std::cout << ch << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

----

# 推荐实践

- 业务代码优先使用 `std::string`。
- 需要与 C API 交互时再使用 `c_str()`。
- 读取字符时若担心越界，优先 `at()`。
- 复杂文本常量优先考虑原始字符串字面量。
- 不要把 `char*` 当成通用字符串类型继续滥用。

----

# 小结

这章真正要建立的判断，不是“会不会拼接字符串”，而是“这里到底该把字符串当对象，还是当底层字符接口来处理”。

- 业务代码默认优先 `std::string`
- 只读参数通常优先 `const std::string&`
- 只有和 C 风格接口交互时才退回 `const char*`

只要你能把这层边界意识建立起来，后面再遇到查找、截取、拼接、传参和生命周期问题时，思路就会清楚很多。
