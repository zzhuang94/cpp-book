# 基础类型

最常见的内置类型包括：

- 整型：`short`、`int`、`long`、`long long`
- 字符类型：`char`
- 浮点型：`float`、`double`
- 布尔型：`bool`
- 空类型：`void`

`C++11` 新增并强化使用的一个重点是：

- `long long` 在工程代码里更加常见
- `char16_t`、`char32_t` 用于更明确的字符编码表示

# 有符号与无符号

```cpp
#include <iostream>

int main() {
    int a = -10;
    unsigned int b = 10;
    std::cout << a << ", " << b << std::endl;
    return 0;
}
```

!> 无符号类型在位运算、协议字段、长度表达里很常见，但也容易引发比较陷阱。

```cpp
int x = -1;
unsigned int y = 1;

// x 会先转换成无符号值，再比较
if (x < y) {
    // 结果可能不是你直觉里的 true
}
```

# sizeof

`sizeof` 用于获取对象或类型占用的字节数：

```cpp
#include <iostream>

int main() {
    std::cout << sizeof(int) << std::endl;
    std::cout << sizeof(double) << std::endl;
    return 0;
}
```

具体大小与平台、编译器、ABI 有关，不要把某个机器上的结果当成语言固定规则。

# 字面量

`C++11` 支持更丰富的字面量形式：

```cpp
int a = 42;
long long b = 1234567890123LL;
double pi = 3.14159;
bool ok = true;
char ch = 'A';
```

还支持原始字符串字面量：

```cpp
#include <iostream>

int main() {
    std::string text = R"(line1
line2
"quoted")";
    std::cout << text << std::endl;
    return 0;
}
```

这对正则、SQL、JSON 片段、多行文本都很方便。

# auto 与基础类型

虽然 `auto` 常用于复杂模板类型，但在基础类型场景里也很常见：

```cpp
auto a = 10;      // int
auto b = 3.14;    // double
auto c = true;    // bool
```

要注意：`auto` 推导结果依赖初始化表达式，不等于“动态类型”。

# decltype

`decltype` 可以从表达式推导出类型，这在泛型和复杂类型编程中很有用。

```cpp
int x = 0;
decltype(x) y = 10; // y 也是 int
```

# bool 语义

`bool` 在旧代码里常被 `int` 替代，但现代代码应直接表达逻辑语义：

```cpp
bool isReady = true;
bool hasError = false;

// 不要继续依赖：这会让接口语义变差。
int flag = 1;
```

# 综合示例

```cpp
#include <iostream>
#include <string>

int main() {
    int count{10};
    long long total = 9000000000LL;
    double ratio = 0.75;
    bool enabled = true;
    char level = 'A';
    std::string name = "cpp11";

    std::cout << "count = " << count << std::endl;
    std::cout << "total = " << total << std::endl;
    std::cout << "ratio = " << ratio << std::endl;
    std::cout << "enabled = " << enabled << std::endl;
    std::cout << "level = " << level << std::endl;
    std::cout << "name = " << name << std::endl;
    return 0;
}
```

# 推荐实践

- 数值范围不明确时，先确认平台差异，不要凭经验硬写。
- 逻辑值使用 `bool`，不要用整数冒充。
- 大整数使用 `long long` 时，字面量记得加 `LL`。
- 复杂初始化下优先使用花括号，避免窄化转换。
- 在泛型或复杂类型场景中合理使用 `auto` 和 `decltype`。

# 小结

基础类型本身没有发生颠覆性变化，但 `C++11` 让类型表达变得更精确、更易读。接手项目时，理解类型不仅是“这个变量存什么”，更是“这个变量的语义、范围和使用方式是什么”。

## 站在其它语言开发者的角度看类型

如果你平时主要写 `Go`、`PHP`、`JS`、`TS`，进入 `C++` 时很容易低估基础类型这一章的重要性。因为你会觉得：

- 我早就知道整数、浮点数、布尔值是什么
- 类型不是语法最简单的一部分吗

但在 `C++` 里，基础类型和下面这些问题经常绑在一起：

- 平台差异
- 窄化转换
- 有符号与无符号比较
- 字面量后缀
- `sizeof` 和对象布局直觉

所以这章真正的重点不是“记住 int 是整数”，而是建立“类型边界会影响程序行为”的习惯。

## 为什么不要想当然地假设类型大小

很多初学者会下意识认为：

- `int` 一定是 4 字节
- `long` 一定比 `int` 大
- 不同平台行为差别不大

这些经验在很多环境下可能“经常成立”，但并不是语言层面的绝对保证。更稳妥的方式是：

- 通过 `sizeof` 理解当前平台
- 真正需要精确范围时再做明确选择
- 不要把某台机器上的实验结果当成全平台定律

## 字面量后缀值得认真看

大整数和特定类型场景里，字面量后缀非常重要：

```cpp
long long id = 1234567890123LL;
unsigned int mask = 42U;
double ratio = 3.14;
float rate = 3.14f;
```

如果后缀和目标类型不一致，可能发生你没注意到的转换。对初学者来说，至少要对 `LL`、`U`、`f` 有基本印象。

## 有符号和无符号为什么经常坑人

看一个更接近真实项目的例子：

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> values{1, 2, 3};
    int index = -1;

    if (index < values.size()) {
        std::cout << "index looks valid" << std::endl;
    }

    return 0;
}
```

这里 `values.size()` 的类型通常是无符号整数。`index` 是有符号整数，混合比较时会触发隐式转换，结果可能完全违背直觉。

这也是为什么警告信息里经常出现 signed/unsigned 相关提示，而且通常值得认真处理。

## `bool` 不只是更好看的 `0/1`

很多旧代码或其它语言迁移思路里，容易把逻辑值看成“反正就是整数真假”。但在现代 `C++` 中，更推荐明确使用 `bool` 表达语义：

```cpp
bool isReady = true;
bool hasPermission = false;
```

这会让：

- 条件表达更清楚
- 接口含义更明确
- 编译器诊断更有帮助

## `decltype` 为什么值得现在就知道

虽然模板章节会进一步展开，但在类型基础阶段先认识 `decltype` 很有价值，因为它告诉你：

> `C++11` 不只是在增加更多类型，而是在增强“表达和推导类型”的能力。

当你开始读标准库、模板和复杂返回类型时，这个意识会非常有用。

## 原始字符串是非常实用的小升级

如果你曾经在其它语言里写过 JSON、SQL、正则或多行文本，原始字符串字面量会立刻让你感到顺手：

```cpp
std::string sql = R"(select * from user where name = "alice")";
```

它的价值在于减少转义噪音，提升可读性，而不是单纯“语法很新”。

## 一个更接近工程代码的例子

```cpp
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> users{"alice", "bob", "carol"};
    std::size_t count = users.size();
    bool hasUsers = (count > 0);
    long long totalRequest = 9000000000LL;

    std::cout << "count = " << count << std::endl;
    std::cout << "hasUsers = " << hasUsers << std::endl;
    std::cout << "totalRequest = " << totalRequest << std::endl;
    return 0;
}
```

这个例子说明，类型的选择常常和容器接口、数值范围、业务语义直接有关，而不是随手写一个最熟的整型就行。

## 常见误区

### 误区 1：基础类型都是平台无关的

不对。至少大小、范围和某些转换行为需要结合平台与编译器理解。

### 误区 2：`unsigned` 一定更安全

不对。它能表达“非负”，但也容易带来比较和减法陷阱。

### 误区 3：`auto` 推导出的基础类型是“动态的”

不对。它仍然在编译期就确定类型。

### 误区 4：只要能编译，类型选择就没问题

不对。很多 bug 来自“类型可以工作，但语义并不合适”。

## 补充建议

- 不确定范围时，不要凭感觉随便选整数类型。
- 混合有符号和无符号类型时格外小心。
- 对逻辑语义使用 `bool`，不要用整数代替。
- 需要表达大整数时，留意字面量后缀和目标类型。
- 读容器接口时留意返回类型，例如 `size()` 往往不是 `int`。
