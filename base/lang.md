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

## 语句块不仅是分组，也是生命周期边界

很多语言里，花括号主要是组织代码结构；在 `C++` 里，它还有一个非常重要的意义：

> 局部对象离开作用域时会自动析构。

例如：

```cpp
#include <iostream>
#include <string>

class Trace {
public:
    explicit Trace(const std::string& name) : name_(name) {
        std::cout << "enter " << name_ << std::endl;
    }

    ~Trace() {
        std::cout << "leave " << name_ << std::endl;
    }

private:
    std::string name_;
};

int main() {
    Trace outer("outer");

    {
        Trace inner("inner");
    }

    return 0;
}
```

这个例子说明，语句块会直接影响对象生命周期，而这会继续影响锁、文件、内存、连接等资源的释放时机。

# auto 的价值和边界

`auto` 不是为了偷懒，而是为了减少冗长样板。它最适合以下场景：

- 右侧类型一眼可见
- 迭代器类型太长
- `lambda` 类型无法显式书写
- 模板返回类型太长

但下面这种情况要多想一步：

```cpp
auto value = getData();
```

如果你根本不知道 `getData()` 返回什么，那么 `auto` 可能会降低可读性。
现代风格不是“无脑全用 `auto`”，而是“在推导清晰时使用 `auto`”。

# using 不只是新语法

很多人第一次看 `using`，会觉得只是 `typedef` 的替代品。
但在模板和复杂别名场景里，`using` 的可读性明显更好：

```cpp
using IdList = std::vector<int>;
using NameMap = std::map<std::string, int>;
```

这会让后面的模板和标准库章节更容易阅读。

# enum class 值得优先使用

传统枚举的两个典型问题是：

- 枚举值容易泄露到外层作用域
- 容易发生不受控制的隐式转换

```cpp
// 旧写法 可能会和别处的 `Red` 冲突。
enum Color { Red, Green, Blue };

// enum class 要求你显式写成 `Color::Red`，这在真实项目里可读性和安全性都更好。
enum class Color { Red, Green, Blue }; 
```

# nullptr 很有用

`nullptr` 看起来只是个小替换，但它解决的是“空指针常量到底是不是整数 0”的历史歧义。

这在函数重载里尤其重要：

```cpp
void log(int);
void log(int*);

log(nullptr);
```

这里会更明确地匹配指针版本。对初学者来说，结论可以先记成一句：

> 只要表达空指针，就统一使用 `nullptr`。

把这几个点合在一起看，你会发现 `C++11` 在基础语法层面做的事情很一致：尽量减少旧式模糊写法，尽量让类型、作用域和意图直接体现在代码表面。这样后面的变量、函数、容器和资源管理章节都会更容易读。

# 小结

基础语法本身并不难，真正需要更新的是你的默认写法。`C++11` 的价值之一，就是把很多过去只能靠经验规避的问题，变成了更清晰、更安全、也更容易阅读的语言表达。