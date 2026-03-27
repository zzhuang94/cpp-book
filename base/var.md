# 变量

变量是最基础的语言元素，但在 `C++11` 中，变量声明方式已经比 `C++98` 灵活得多。你需要关注的不只是“怎么定义变量”，还包括“变量的类型如何推导、生命周期如何管理、初始化是否安全”。

# 基本声明

```cpp
int age = 30;
double score = 95.5;
std::string name = "Alice";
```

变量声明由三部分构成：

- 类型
- 名称
- 初始化表达式

# 初始化

`C++11` 推荐更统一的初始化风格：

```cpp
int a = 10;
int b(20);
int c{30};
```

花括号初始化的优势之一是防止窄化：

```cpp
// int x{3.14}; // 编译错误
int y = 3.14;   // 能编译，但会截断
```

# `auto`

`auto` 是 `C++11` 中最重要的变量声明升级之一。

```cpp
auto x = 10;           // int
auto y = 3.14;         // double
auto name = "hello";   // const char*
```

它尤其适合：

- 迭代器
- 模板返回值
- `lambda` 类型
- 非常长的标准库类型

例如：

```cpp
std::vector<int> values{1, 2, 3};
auto it = values.begin();
```

# `auto` 限制

下面这种写法未必好：

```cpp
auto ok = true;
auto count = 100;
```

如果右侧表达式已经非常简单，显式类型和 `auto` 都可以。关键标准不是“是否能省字”，而是“是否更清楚”。

# 作用域

```cpp
#include <iostream>

int main() {
    int outer = 10;

    {
        int inner = 20;
        std::cout << outer + inner << std::endl;
    }

    return 0;
}
```

变量离开作用域后会被销毁。对于对象类型，这意味着析构函数会自动执行，这也是 RAII 的基础。

# `static` 局部变量

局部 `static` 变量只初始化一次，生命周期持续到程序结束：

```cpp
#include <iostream>

void visit() {
    static int count = 0;
    ++count;
    std::cout << "visit count = " << count << std::endl;
}

int main() {
    visit();
    visit();
    visit();
    return 0;
}
```

# `decltype`

`decltype` 常与变量声明配合使用：

```cpp
int x = 42;
decltype(x) y = 100;
```

它在模板和泛型编程中非常重要，因为可以根据已有表达式得到精确类型。

# 综合示例

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{10, 20, 30};
    auto total = 0;

    for (auto n : nums) {
        total += n;
    }

    decltype(total) average = total / static_cast<int>(nums.size());

    std::cout << "total = " << total << std::endl;
    std::cout << "average = " << average << std::endl;
    return 0;
}
```

# 推荐实践

- 定义变量时尽量立即初始化。
- 优先使用花括号初始化。
- 遇到复杂模板类型时优先使用 `auto`。
- 简单字面量场景不必盲目全部改成 `auto`。
- 缩小变量作用域，尽量在真正需要时再定义。
- 不要复用同一个变量承载多阶段、不同语义的值。

# 小结

在现代 `C++` 里，变量声明不仅是“开一个内存格子”，更是在表达类型、生命周期和意图。`C++11` 通过 `auto`、统一初始化、`decltype` 等机制，让变量写法既更安全也更接近真实业务语义。

## 为什么变量这一章在 C++ 里比想象中重要

在很多语言里，变量更多只是“放个值”。但在 `C++` 中，变量还会额外携带几层信息：

- 生命周期有多长
- 是否会触发构造和析构
- 是否发生复制
- 类型是显式写出还是通过 `auto` 推导

这意味着变量写法本身就会影响程序的行为和性能。

## 缩小作用域是很重要的现代习惯

例如：

```cpp
int main() {
    std::vector<int> values{1, 2, 3};

    for (std::size_t i = 0; i < values.size(); ++i) {
        int current = values[i];
        std::cout << current << std::endl;
    }

    return 0;
}
```

这里的 `current` 只在循环体里有效，这样做的好处是：

- 变量语义更集中
- 不容易被误用到后面的逻辑里
- 生命周期更短，心智负担更小

## 定义时初始化在 C++ 里尤其重要

因为局部基础类型不会自动帮你清零，未初始化读取是非常危险的。因此“先声明，后面再想办法赋值”在 `C++` 里通常不是一个好默认习惯。

更推荐：

```cpp
int retryCount{0};
std::string name{"guest"};
```

## `auto` 的一个典型使用边界

看下面两个例子：

```cpp
auto it = values.begin();
```

这是非常典型、很适合使用 `auto` 的场景。

```cpp
auto userCount = 3;
```

这类简单字面量场景则没必要为了“现代”而机械使用 `auto`。好的判断标准是：它是否真的提升了可读性。

## 变量名在现代 C++ 里更重要了

随着 `auto`、模板和标准库类型大量出现，变量名往往承担了更多语义信息。例如：

- `it` 适合作为短生命周期迭代器
- `userName` 比 `s` 更清楚
- `totalCount` 比 `n` 更容易维护

尤其在读复杂泛型代码时，变量名常常比显式类型更能帮助理解业务语义。

## 常见误区

### 误区 1：`auto` 越多越现代

不对。现代风格强调的是清晰，而不是尽量少写类型。

### 误区 2：变量先声明出来，后面总会赋值

不安全。特别是基础类型，这会增加未初始化使用风险。

### 误区 3：局部变量作用域大一点没关系

不建议。作用域越大，越容易被错误复用。

## 补充建议

- 变量尽量在第一次需要时再定义。
- 一旦定义就尽量初始化。
- 复杂模板和迭代器场景合理使用 `auto`。
- 不要让同一个变量跨越太多语义阶段。
- 名称优先表达业务含义，而不是图省事写单字母。
