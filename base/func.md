# 函数基础

函数最基本的作用，是把一段可重复使用的逻辑封装成一个清晰的接口。

```cpp
#include <iostream>

int add(int a, int b) {
    return a + b;
}

int main() {
    std::cout << add(3, 4) << std::endl;
    return 0;
}
```

调用者通常只需要先关心三件事：

- 函数名是什么
- 需要传入什么参数
- 会返回什么结果

这种“先看接口，再看实现”的阅读方式很重要。

> 后面学习类、模板和标准库时，你会越来越频繁地先阅读函数签名，再判断这个接口该怎么用。

----

# 返回值与 void

不是所有函数都必须“算出一个结果”。有些函数只是完成一个动作，比如打印、修改对象、写日志。

```cpp
#include <iostream>

void printLine() {
    std::cout << "hello" << std::endl;
}

int square(int x) {
    return x * x;
}
```

- `void` 表示函数不返回值
- 非 `void` 函数通常需要通过 `return` 返回结果

----

# 函数声明

很多时候，函数会先声明、后定义：

```cpp
#include <iostream>

int add(int a, int b);  // 声明

int main() {
    std::cout << add(3, 4) << std::endl;
    return 0;
}

int add(int a, int b) { // 定义
    return a + b;
}
```

- 声明告诉编译器“这个函数存在，签名是什么”；定义才真正提供函数体。
- 如果没有前面的声明，编译器在看到 `add(3, 4)` 时，还不知道 `add` 是什么，就无法通过编译。
- 当项目开始拆成多个源文件时，声明通常放在头文件中，定义放在 `.cpp` 文件中。这也是理解后续代码组织方式的基础。

----

# 参数传递方式

函数参数不只是“把值传进去”，它还表达了接口语义。

```cpp
#include <iostream>
#include <string>

void printValue(int x) {
    std::cout << x << std::endl;
}
void printName(const std::string& name) {
    std::cout << name << std::endl;
}
void increase(int& x) {
    ++x;
}
```

常见形式：

- `T x`：按值传递，会复制一份参数，适合小对象或就是希望得到副本
- `const T& x`：按常量引用传递，不复制，适合较大的对象，也能明确表示“只读”
- `T& x`：按引用传递，可以修改实参，适合原地修改

可以通过下面这个例子直接看出差别：

```cpp
#include <iostream>

void changeByValue(int x) {
    x = 100;
}
void changeByRef(int& x) {
    x = 100;
}
int main() {
    int n = 10;
    changeByValue(n);
    std::cout << n << std::endl;  // 10
    changeByRef(n);
    std::cout << n << std::endl;  // 100
    return 0;
}
```

> 参数形式本身就是接口的一部分。看到 `const T&`，你应该立刻想到“只读”；看到 `T&`，你就要意识到“它可能改我传进去的对象”。

----

# 返回语句

`return` 不只是“把值送回去”，它还会立即结束当前函数。

```cpp
int absValue(int x) {
    if (x >= 0) {
        return x;
    }
    return -x;
}
```

这类写法很常见。满足条件就直接返回，不满足再继续往下执行。

对于 `void` 函数，也可以单独写 `return;` 提前结束：

```cpp
void printPositive(int x) {
    if (x <= 0) {
        return;
    }
    std::cout << x << std::endl;
}
```

> 这种“尽早返回”的思路，往往比把很多分支层层嵌套起来更清楚。

----

# 默认参数

```cpp
#include <iostream>

void greet(const char* name = "guest") {
    std::cout << "hello, " << name << std::endl;
}
int main() {
    greet();
    greet("alice");
    return 0;
}
```

默认参数的作用是：调用者在省略一部分实参时，函数仍然可以正常工作。

需要注意几个规则：

- 默认参数通常从右向左连续提供，不能中间断开
- 默认值一般只在函数声明里写一次，避免重复定义
- 如果和函数重载一起使用，要注意是否会造成调用歧义

```cpp
void connect(const char* host, int port = 80);
// void connect(const char* host = "localhost", int port); // 不合法
```

默认参数适合“多数调用使用同一默认行为，少数调用再手动覆盖”的场景。

----

# 函数重载

`C++` 允许多个函数使用同一个名字，只要参数列表不同，这叫函数重载。

```cpp
int add(int a, int b) {
    return a + b;
}

double add(double a, double b) {
    return a + b;
}
```

基础阶段先记住两点就够：

- 编译器会根据参数列表选择合适版本
- 一旦重载和默认参数、隐式转换混在一起，接口就会变复杂

因此这里先建立概念，不把规则展开太深。  
更系统的内容放在后面的 `函数重载` 专章里。

----

# 提炼函数

> 函数不只是语法知识，它还是组织代码的核心工具。

如果你发现一段代码出现下面这些情况，通常就可以考虑提炼成函数：
- 逻辑会重复出现
- 一段代码太长，读起来不容易把握重点
- 这段逻辑本身有清晰名字
- 你希望把“做什么”和“怎么做”分开

例如：

```cpp
bool isEven(int x) {
    return x % 2 == 0;
}
```

有了这个函数后，调用处通常会比直接写表达式更容易读：

```cpp
if (isEven(n)) {
    std::cout << "even" << std::endl;
}
```

----

# 综合示例

```cpp
#include <iostream>
#include <string>

bool isAdult(int age) {
    return age >= 18;
}

void greet(const std::string& name, bool polite = true) {
    if (polite) {
        std::cout << "hello, " << name << std::endl;
        return;
    }
    std::cout << name << std::endl;
}

int main() {
    int age = 20;
    std::cout << isAdult(age) << std::endl;
    greet("alice");
    greet("bob", false);
    return 0;
}
```

这段代码里同时出现了：

- 返回值函数
- `void` 函数
- `const T&` 参数
- 默认参数
- 提前 `return`

把这些基础点放在一起看，比零散记语法更容易形成整体理解。

----

# 推荐实践

- 先把函数签名读清楚，再去看实现。
- 一个函数尽量只做一件事情，名字也尽量直接表达意图。
- 能返回结果时，优先返回结果，不要一上来就把逻辑写成“直接输出”。
- 小对象可以按值传递，较大的只读对象优先考虑 `const T&`。
- 只有确实需要修改外部对象时，再使用引用参数。
- 默认参数保持简单，避免和重载一起制造歧义。
- 函数体过长或重复逻辑出现时，及时提炼成独立函数。
- 看到 `return` 时，不只要看“返回了什么”，还要注意“函数是否在这里提前结束”。

----

# 小结

函数这一章真正需要建立的判断，是“这个接口到底想表达什么”。

- 返回值在表达“我要产出结果”
- 参数形式在表达“我是复制、只读还是可修改”
- 函数名在表达“这段逻辑的职责是什么”

只要你不把函数理解成一堆零散语法，而是把它看成“组织逻辑和表达接口意图”的工具，后面学习类、模板和标准库时就会顺很多。