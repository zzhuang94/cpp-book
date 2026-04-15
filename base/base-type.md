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

----

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

----

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

----

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

----

# 类型推导

`auto` 和 `decltype` 会在后面的 `变量` 一章里集中展开。  
在基础类型这里，你只需要先记住：

- 基础类型也会参与类型推导
- 类型推导不等于动态类型
- 推导结果仍然受初始化表达式和 `C++` 类型规则约束

也就是说，基础类型这一章更关注“类型本身的语义”，而不是“声明方式的变化”。

----

# bool 语义

`bool` 在旧代码里常被 `int` 替代，但现代代码应直接表达逻辑语义：

```cpp
bool isReady = true;
bool hasError = false;

// 不要继续依赖：这会让接口语义变差。
int flag = 1;
```

----

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

----

# 推荐实践

- 数值范围不明确时，先确认平台差异，不要凭经验硬写。
- 逻辑值使用 `bool`，不要用整数冒充。
- 大整数使用 `long long` 时，字面量记得加 `LL`。
- 复杂初始化下优先使用花括号，避免窄化转换。
- 在泛型或复杂类型场景中合理使用 `auto` 和 `decltype`。

----

# 小结

- 基础类型这一章真正要建立的，不是“背出有哪些类型”，而是对范围、语义和隐式转换保持敏感。
- 现代 `C++` 更鼓励你把类型写得明确、读得明白，这样后面面对容器、模板和接口设计时才不容易踩坑。
