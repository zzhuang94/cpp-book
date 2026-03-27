# 运算符

运算符本身不是 `C++11` 最“新”的部分，但很多 `C++11` 特性会改变你书写表达式的方式，比如 `nullptr`、移动语义、强类型枚举、`decltype`、`static_cast` 的使用习惯等。

# 算术运算符

常见算术运算符包括：

- `+`
- `-`
- `*`
- `/`
- `%`

```cpp
#include <iostream>

int main() {
    int a = 10;
    int b = 3;

    std::cout << a + b << std::endl;
    std::cout << a - b << std::endl;
    std::cout << a * b << std::endl;
    std::cout << a / b << std::endl;
    std::cout << a % b << std::endl;
    return 0;
}
```

要注意整数除法会直接截断小数部分。

# 关系运算符

```cpp
int x = 10;
int y = 20;

bool r1 = (x < y);
bool r2 = (x == y);
bool r3 = (x != y);
```

关系表达式的结果是 `bool`，现代代码应直接围绕布尔语义组织逻辑，而不是继续依赖整数真假值。

# 逻辑运算符

- `&&`
- `||`
- `!`

```cpp
bool isAdmin = true;
bool isActive = false;

bool canAccess = isAdmin && isActive;
```

短路行为仍然存在：

- `&&` 左边为假时，右边不计算
- `||` 左边为真时，右边不计算

# 自增自减

```cpp
int i = 0;
++i;
i++;
```

在迭代器场景中，旧代码常强调优先 `++it` 而不是 `it++`，因为前者通常避免额外临时对象。现代编译器很多时候能优化，但这个习惯仍然值得保留。

# 赋值与复合赋值

```cpp
int total = 0;
total += 10;
total -= 2;
total *= 3;
```

# 位运算符

位运算在底层开发、权限控制、协议解析中常见：

- `&`
- `|`
- `^`
- `~`
- `<<`
- `>>`

```cpp
#include <iostream>

int main() {
    unsigned int flags = 0b0011;
    flags |= 0b0100;
    std::cout << flags << std::endl;
    return 0;
}
```

# 条件运算符

```cpp
int age = 20;
const char* category = (age >= 18) ? "adult" : "minor";
```

条件运算符适合非常简单的二选一场景，逻辑一旦复杂，应优先使用 `if/else`。

# 类型转换运算符

老代码里常见 C 风格转换：

```cpp
double x = 3.14;
int y = (int)x;
```

`C++` 更推荐显式转换运算符：

```cpp
int y = static_cast<int>(x);
```

原因是：

- 更容易搜索和审查
- 语义更明确
- 能区分不同转换目的

# `nullptr` 重载

`C++11` 中一个和运算符/表达式密切相关的变化是 `nullptr`：

```cpp
#include <iostream>

void f(int) {
    std::cout << "int" << std::endl;
}

void f(int*) {
    std::cout << "pointer" << std::endl;
}

int main() {
    f(nullptr);
    return 0;
}
```

这避免了 `NULL` 可能被当成整数 `0` 的歧义。

# 综合示例

```cpp
#include <iostream>

enum class Status {
    Ok,
    Failed
};

int main() {
    int a = 8;
    int b = 3;
    bool ok = (a > b) && (b != 0);
    int result = ok ? (a / b) : 0;

    std::cout << "ok = " << ok << std::endl;
    std::cout << "result = " << result << std::endl;
    std::cout << "status = " << static_cast<int>(Status::Ok) << std::endl;
    return 0;
}
```

# 推荐实践

- 逻辑表达式直接使用 `bool`，避免“0/1 充当真假”。
- 类型转换优先使用 `static_cast` 等 C++ 风格转换。
- 指针空值统一使用 `nullptr`。
- 简单场景用条件运算符，复杂条件用 `if/else`。
- 位运算只在确有需求时使用，并配套清晰注释或封装。

# 小结

运算符本身并没有因为 `C++11` 而被推翻，但现代 `C++` 更强调表达式的可读性和类型安全。你要学的不只是“能写出结果”，而是“让表达式尽可能少歧义、少隐患”。

## 其它语言开发者最容易忽略的点

在很多语言里，运算符问题通常不会成为“项目阅读门槛”。但在 `C++` 中，运算符经常和这些内容纠缠在一起：

- 隐式类型转换
- 有符号与无符号比较
- 指针与空指针语义
- 自定义类型的运算符重载

所以这章真正重要的不是把每个符号背下来，而是学会警惕“看起来很普通的表达式，背后可能有复杂规则”。

## `==`、`=` 和编译警告

条件判断里最经典的低级错误就是把比较写成赋值：

```cpp
if (value = 0) {
}
```

现代实践里非常建议始终打开警告，就是为了尽早抓到这种问题。对 `C++` 来说，警告并不是可有可无的提示，而是非常重要的防线。

## 整数除法是阅读表达式时要随时意识到的点

```cpp
int a = 5;
int b = 2;
std::cout << a / b << std::endl;
```

结果是 `2`，不是 `2.5`。看似简单，但在统计、平均值、比例计算中非常容易埋下错误。

## C 风格转换为什么不推荐

```cpp
int y = (int)x;
```

这类写法虽然短，但信息量太少。现代 `C++` 更推荐：

```cpp
int y = static_cast<int>(x);
```

原因不是形式主义，而是它更清楚地告诉读者：“这里发生了一个显式转换，而且是某种特定类别的转换。”

## 布尔逻辑优先写得像布尔逻辑

旧风格代码里经常会看到大量“0/1 即真假”的写法，但现代代码更推荐显式使用 `bool` 变量承接复杂条件：

```cpp
bool isReady = (status == 200);
bool hasPermission = (role == "admin");

if (isReady && hasPermission) {
}
```

这样做的价值是让条件表达式更接近业务语义，而不是只剩下一串操作符。

## 常见误区

### 误区 1：运算符问题只是初学语法

不对。很多类型转换和表达式 bug 都来自对运算规则理解不完整。

### 误区 2：C 风格转换和 `static_cast` 没什么区别

不建议这么看。后者更清晰、更易审查。

### 误区 3：逻辑表达式能跑就行

不对。表达式越复杂，越应该拆成具名布尔变量。

## 补充建议

- 表达式复杂时优先拆变量，而不是一行里堆太多操作符。
- 看到整数除法时主动确认是否符合预期。
- 类型转换优先使用 C++ 风格转换。
- 空指针统一使用 `nullptr`。
- 混合不同整数类型时特别留意警告。
