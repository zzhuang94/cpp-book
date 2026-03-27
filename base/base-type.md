# 基础类型

`C++` 的基础类型看起来和 10 年前差别不大，但到了 `C++11`，类型系统周围的语法工具明显更强了。你需要同时掌握“基础类型本身”与“如何更现代地使用类型”。

## 常见基础类型

最常见的内置类型包括：

- 整型：`short`、`int`、`long`、`long long`
- 字符类型：`char`
- 浮点型：`float`、`double`
- 布尔型：`bool`
- 空类型：`void`

`C++11` 新增并强化使用的一个重点是：

- `long long` 在工程代码里更加常见
- `char16_t`、`char32_t` 用于更明确的字符编码表示

## 有符号与无符号

```cpp
#include <iostream>

int main() {
    int a = -10;
    unsigned int b = 10;
    std::cout << a << ", " << b << std::endl;
    return 0;
}
```

无符号类型在位运算、协议字段、长度表达里很常见，但也容易引发比较陷阱。

```cpp
int x = -1;
unsigned int y = 1;

// x 会先转换成无符号值，再比较
if (x < y) {
    // 结果可能不是你直觉里的 true
}
```

## `sizeof`

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

## 字面量

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

## `auto` 与基础类型

虽然 `auto` 常用于复杂模板类型，但在基础类型场景里也很常见：

```cpp
auto a = 10;      // int
auto b = 3.14;    // double
auto c = true;    // bool
```

要注意：`auto` 推导结果依赖初始化表达式，不等于“动态类型”。

## `decltype`

`decltype` 可以从表达式推导出类型，这在泛型和复杂类型编程中很有用。

```cpp
int x = 0;
decltype(x) y = 10; // y 也是 int
```

## `bool` 的工程意义

`bool` 在旧代码里常被 `int` 替代，但现代代码应直接表达逻辑语义：

```cpp
bool isReady = true;
bool hasError = false;
```

不要继续依赖：

```cpp
int flag = 1;
```

这会让接口语义变差。

## 一个综合示例

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

## 推荐实践

- 数值范围不明确时，先确认平台差异，不要凭经验硬写。
- 逻辑值使用 `bool`，不要用整数冒充。
- 大整数使用 `long long` 时，字面量记得加 `LL`。
- 复杂初始化下优先使用花括号，避免窄化转换。
- 在泛型或复杂类型场景中合理使用 `auto` 和 `decltype`。

## 小结

基础类型本身没有发生颠覆性变化，但 `C++11` 让类型表达变得更精确、更易读。接手项目时，理解类型不仅是“这个变量存什么”，更是“这个变量的语义、范围和使用方式是什么”。
