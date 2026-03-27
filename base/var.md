# 变量

- 变量是最基础的语言元素，但在 `C++11` 中，变量声明方式已经比 `C++98` 灵活得多。
- 你需要关注的不只是“怎么定义变量”，还包括“变量的类型如何推导、生命周期如何管理、初始化是否安全”。

```cpp
int age = 30;
double score = 95.5;
std::string name = "Alice";
```

> 变量声明由三部分构成：`类型` `名称` `初始化表达式`

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

# auto

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
    // 此处无法使用 innner
    return 0;
}
```

> 变量离开作用域后会被销毁。对于对象类型，这意味着析构函数会自动执行，这也是 RAII 的基础。

# static 局部变量

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

# decltype

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

- 变量写法在 `C++` 里不只是“把值放进去”，它还会影响初始化时机、作用域边界和后续阅读成本。
- 尤其当你开始使用 `auto` 和 `decltype` 时，变量名、初始化表达式和声明位置会一起决定这段代码是不是足够清楚。

# 推荐实践

- 定义变量时尽量立即初始化。
- *优先使用花括号初始化。*
- 遇到复杂模板类型时优先使用 `auto`。
- 简单字面量场景不必盲目全部改成 `auto`。
- 缩小变量作用域，尽量在真正需要时再定义。
- 不要复用同一个变量承载多阶段、不同语义的值。

# 小结

- 变量这一章的关键，不是把声明语法写对就结束，而是让变量的类型、生命周期和用途一眼可见。
- 现代 `C++` 提供了更好的写法，但判断标准始终只有一个：是不是更清楚、更安全。
