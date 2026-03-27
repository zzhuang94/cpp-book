# 运算符

运算符本身不是 `C++11` 最“新”的部分，但很多 `C++11` 特性会改变你书写表达式的方式，比如 `nullptr`、移动语义、强类型枚举、`decltype`、`static_cast` 的使用习惯等。

## 算术运算符

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

## 关系运算符

```cpp
int x = 10;
int y = 20;

bool r1 = (x < y);
bool r2 = (x == y);
bool r3 = (x != y);
```

关系表达式的结果是 `bool`，现代代码应直接围绕布尔语义组织逻辑，而不是继续依赖整数真假值。

## 逻辑运算符

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

## 自增自减

```cpp
int i = 0;
++i;
i++;
```

在迭代器场景中，旧代码常强调优先 `++it` 而不是 `it++`，因为前者通常避免额外临时对象。现代编译器很多时候能优化，但这个习惯仍然值得保留。

## 赋值与复合赋值

```cpp
int total = 0;
total += 10;
total -= 2;
total *= 3;
```

## 位运算符

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

## 条件运算符

```cpp
int age = 20;
const char* category = (age >= 18) ? "adult" : "minor";
```

条件运算符适合非常简单的二选一场景，逻辑一旦复杂，应优先使用 `if/else`。

## 类型转换运算符

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

## `nullptr` 参与重载解析

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

## 一个综合示例

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

## 推荐实践

- 逻辑表达式直接使用 `bool`，避免“0/1 充当真假”。
- 类型转换优先使用 `static_cast` 等 C++ 风格转换。
- 指针空值统一使用 `nullptr`。
- 简单场景用条件运算符，复杂条件用 `if/else`。
- 位运算只在确有需求时使用，并配套清晰注释或封装。

## 小结

运算符本身并没有因为 `C++11` 而被推翻，但现代 `C++` 更强调表达式的可读性和类型安全。你要学的不只是“能写出结果”，而是“让表达式尽可能少歧义、少隐患”。
