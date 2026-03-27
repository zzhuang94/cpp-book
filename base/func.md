# 函数

函数在 `C++11` 中的变化非常值得重点学习。旧时代你主要关注参数、返回值、重载、内联；到了 `C++11`，你还必须理解 `lambda`、尾置返回类型、`std::function`、右值引用参数以及可变参数模板的基本思路。

# 函数定义

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

默认参数在旧版 `C++` 中就存在，但仍然是很常用的接口简化手段。

# 函数重载

```cpp
#include <iostream>

int add(int a, int b) {
    return a + b;
}

double add(double a, double b) {
    return a + b;
}
```

重载非常强大，但也容易因为隐式转换而引入歧义。`C++11` 通过 `nullptr`、`=delete` 等机制让接口设计更精确，这在后续面向对象章节会反复出现。

# 内联函数

```cpp
inline int square(int x) {
    return x * x;
}
```

`inline` 不应简单理解为“强制内联优化”，它还有链接层面的含义。性能是否真正内联，最终由编译器决定。

# 尾置返回类型

当返回类型依赖复杂表达式时，`C++11` 可以使用尾置返回类型：

```cpp
auto add(int a, int b) -> int {
    return a + b;
}
```

这个特性在模板编程和 `decltype` 配合时尤其有用。

# `lambda`

这是 `C++11` 函数相关最重要的新特性之一。

```cpp
#include <iostream>

int main() {
    auto add = [](int a, int b) {
        return a + b;
    };

    std::cout << add(2, 5) << std::endl;
    return 0;
}
```

`lambda` 的价值在于：

- 可以就地定义短小逻辑
- 非常适合与算法库配合
- 避免为了简单回调额外定义函数对象类

# 捕获列表

```cpp
#include <iostream>

int main() {
    int base = 10;

    auto addBase = [base](int x) {
        return base + x;
    };

    std::cout << addBase(5) << std::endl;
    return 0;
}
```

常见捕获形式：

- `[x]` 按值捕获
- `[&x]` 按引用捕获
- `[=]` 按值捕获所有外部变量
- `[&]` 按引用捕获所有外部变量

按引用捕获很方便，但要小心生命周期问题。

# `std::function`

当你需要“存储一个可调用对象”时，`std::function` 非常实用：

```cpp
#include <functional>
#include <iostream>

int main() {
    std::function<int(int, int)> op = [](int a, int b) {
        return a + b;
    };

    std::cout << op(3, 7) << std::endl;
    return 0;
}
```

它能统一包装：

- 普通函数
- `lambda`
- 函数对象
- `bind` 结果

# 右值引用参数

`C++11` 引入右值引用后，函数参数的设计也升级了：

```cpp
#include <iostream>
#include <string>

void setName(std::string&& name) {
    std::cout << name << std::endl;
}

int main() {
    setName("temporary");
    return 0;
}
```

这和移动语义密切相关，后续在引用和类设计章节会重点展开。

# 可变参数模板

`C++11` 支持模板参数包，这是很多现代库设计的基础：

```cpp
#include <iostream>

void print() {
    std::cout << std::endl;
}

template <typename T, typename... Args>
void print(const T& first, const Args&... rest) {
    std::cout << first << " ";
    print(rest...);
}

int main() {
    print(1, "hello", 3.14);
    return 0;
}
```

# 综合示例

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{1, 2, 3, 4, 5};
    int threshold = 3;

    auto count = std::count_if(nums.begin(), nums.end(), [threshold](int x) {
        return x > threshold;
    });

    std::cout << count << std::endl;
    return 0;
}
```

# 推荐实践

- 短小回调逻辑优先使用 `lambda`。
- 参数大对象优先使用 `const T&`，避免不必要复制。
- 只有当返回类型不清晰或依赖表达式时，再使用尾置返回类型。
- 需要统一保存回调时使用 `std::function`。
- 捕获引用时始终检查被捕获对象的生命周期。

# 小结

函数在 `C++11` 中已经不只是“定义一段可调用代码”那么简单。`lambda`、`std::function` 和右值引用让函数式写法、泛型写法和性能语义都比旧时代更强，也更值得重点掌握。
