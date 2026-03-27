# 基础语法

`C++` 的基础语法你大概率并不陌生，真正需要重建的是“哪些老语法仍然存在，但在 `C++11` 里有了更好的表达方式”。

## 程序的基本结构

一个最小 `C++` 程序通常包含：

- 头文件 `#include`
- 命名空间限定，如 `std::cout`
- `main()` 入口函数
- 语句块与分号

```cpp
#include <iostream>

int main() {
    std::cout << "Hello, C++11" << std::endl;
    return 0;
}
```

## 语句块与作用域

花括号 `{}` 定义作用域。局部变量只在所在语句块内有效。

```cpp
#include <iostream>

int main() {
    int x = 10;

    {
        int y = 20;
        std::cout << x + y << std::endl;
    }

    // 这里不能访问 y
    return 0;
}
```

`C++11` 没有改变作用域规则，但很多新特性都依赖“对象生命周期”和“离开作用域自动析构”的思维，这正是 RAII 的基础。

## 命名空间

`C++98` 时很多教程喜欢直接写：

```cpp
using namespace std;
```

这在小示例里没问题，但在真实项目里容易带来命名冲突。更推荐：

- 直接写 `std::string`
- 或只局部引入需要的名字：`using std::cout;`

## 初始化方式的升级

`C++11` 一个很重要的变化是统一初始化语法：

```cpp
int a = 10;
int b(20);
int c{30};
```

相比旧写法，花括号初始化的优势在于更统一，而且能阻止某些危险的窄化转换。

```cpp
int x = 3.14;   // 允许，但会丢失精度
// int y{3.14}; // 编译错误，防止窄化
```

## `auto` 带来的语法变化

以前很多类型必须完整写出来，特别是模板和迭代器：

```cpp
std::vector<int>::iterator it = values.begin();
```

`C++11` 可以写成：

```cpp
auto it = values.begin();
```

这不是偷懒，而是把注意力从“类型拼写”转向“表达意图”。

## `using` 取代部分 `typedef`

旧写法：

```cpp
typedef unsigned long ulong;
```

`C++11` 推荐：

```cpp
using ulong = unsigned long;
```

`using` 在模板别名中尤其清晰。

## `enum class`

传统枚举会把名字暴露到外层作用域，而且可能发生隐式转换。

```cpp
enum Color { Red, Green, Blue };
```

`C++11` 引入强类型枚举：

```cpp
enum class Color { Red, Green, Blue };
```

使用时需要显式限定：

```cpp
Color c = Color::Red;
```

这能减少命名污染，也能避免很多隐式转换问题。

## `nullptr`

旧代码里常见：

```cpp
int* p = NULL;
```

`C++11` 应改成：

```cpp
int* p = nullptr;
```

因为 `nullptr` 是专门表示空指针的字面量，能避免重载解析歧义。

## 一个综合示例

```cpp
#include <iostream>
#include <vector>

enum class LogLevel {
    Info,
    Warning,
    Error
};

int main() {
    std::vector<int> values{1, 2, 3};
    auto count = values.size();
    int* ptr = nullptr;

    std::cout << "count = " << count << std::endl;
    std::cout << "ptr is null? " << (ptr == nullptr) << std::endl;
    std::cout << "level = " << static_cast<int>(LogLevel::Warning) << std::endl;
    return 0;
}
```

## 推荐实践

- 优先使用花括号初始化。
- 优先使用 `nullptr`，不要再写 `NULL`。
- 模板或迭代器类型太长时优先使用 `auto`。
- 避免在头文件中写 `using namespace std;`。
- 新代码优先使用 `using` 而不是 `typedef`。
- 枚举若有明确语义边界，优先使用 `enum class`。

## 小结

基础语法本身并不难，难的是更新你的默认写法。`C++11` 的价值之一，就是把很多过去“靠经验避免踩坑”的事情，变成了语言层面更清晰、更安全的表达方式。
