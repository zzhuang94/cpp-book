# 基础语法

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

----

# C++11

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

> `C++11` 没有改变作用域规则，但很多新特性都依赖“对象生命周期”和“离开作用域自动析构”的思维，这正是 RAII 的基础。

## 命名空间

`C++98` 时很多教程喜欢直接写：

```cpp
using namespace std;
```

这在小示例里没问题，但在真实项目里容易带来命名冲突。更推荐：

- 直接写 `std::string`
- 或只局部引入需要的名字：`using std::cout;`

## 初始化升级

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

## auto

以前很多类型必须完整写出来，特别是模板和迭代器：

```cpp
// 
std::vector<int>::iterator it = values.begin();
// C++11
auto it = values.begin();
```

这不是偷懒，而是把注意力从“类型拼写”转向“表达意图”。

## using

```cpp
// 旧写法
typedef unsigned long ulong;
// C++11
using ulong = unsigned long;
```

## enum class

```cpp
// 传统枚举会把名字暴露到外层作用域，而且可能发生隐式转换。
enum Color { Red, Green, Blue };

// C++11` 引入强类型枚举：使用时需要显式限定：这能减少命名污染，也能避免很多隐式转换问题。
enum class Color { Red, Green, Blue };
Color c = Color::Red;
```

## nullptr

```cpp
// 旧代码里常见：
int* p = NULL;
// C++11 改成：因为 `nullptr` 是专门表示空指针的字面量，能避免重载解析歧义。
int* p = nullptr;
```

## 综合示例

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

## 从其它语言迁移时的理解方式

如果你来自 `Go`、`PHP`、`TS`，基础语法层面最容易产生的误解通常不是“看不懂”，而是“以为自己懂了”。例如：

- 你看到花括号，以为它只是普通代码块，却忽略了作用域和对象析构时机。
- 你看到 `auto`，以为它像动态类型，其实它仍然是静态类型推导。
- 你看到 `enum class`，以为只是更长的枚举写法，却忽略了它对作用域和隐式转换的限制。

所以这一章最重要的不是背更多语法，而是让你知道 `C++11` 在基础语法层面开始鼓励怎样的默认风格。

后面的 `变量`、`常量`、`指针` 等章节，会继续把这些点展开：

- 作用域和生命周期
- `auto` 的使用边界
- `using` 的工程价值
- `enum class` 的作用域与类型安全
- `nullptr` 的重载语义

这一章先做“建立地图”，不把这些点全部展开到底，避免和后续章节重复过深。

----

# 小结

基础语法本身并不难，真正需要更新的是你的默认写法。`C++11` 的价值之一，就是把很多过去只能靠经验规避的问题，变成了更清晰、更安全、也更容易阅读的语言表达。