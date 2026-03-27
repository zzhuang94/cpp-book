# 循环

循环结构在 `C++98` 与 `C++11` 之间看起来变化不大，但 `C++11` 增加的范围 `for` 显著改变了容器遍历的默认写法。你接手现代 `C++` 项目时，这一章会非常高频。

# `while`

```cpp
#include <iostream>

int main() {
    int i = 0;

    while (i < 3) {
        std::cout << i << std::endl;
        ++i;
    }

    return 0;
}
```

适合“循环次数不固定，只要条件满足就继续”的场景。

# `do...while`

```cpp
#include <iostream>

int main() {
    int i = 0;

    do {
        std::cout << i << std::endl;
        ++i;
    } while (i < 3);

    return 0;
}
```

特点是循环体至少执行一次。

# 传统 `for`

```cpp
#include <iostream>

int main() {
    for (int i = 0; i < 5; ++i) {
        std::cout << i << std::endl;
    }

    return 0;
}
```

这类写法仍然非常常见，尤其适合：

- 需要索引
- 明确知道起始、结束和步长
- 处理裸数组或数值循环

# 范围 `for`

这是 `C++11` 循环部分最重要的新增能力。

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> values{10, 20, 30};

    for (int value : values) {
        std::cout << value << std::endl;
    }

    return 0;
}
```

进一步推荐：

```cpp
for (auto value : values) {
    std::cout << value << std::endl;
}
```

# 遍历方式

这一点非常重要。

## 值遍历

```cpp
for (auto value : values) {
}
```

每次循环都会复制一个元素。

## 引用遍历

```cpp
for (auto& value : values) {
    value *= 2;
}
```

## 常量引用遍历

```cpp
for (const auto& value : values) {
    std::cout << value << std::endl;
}
```

这通常是遍历复杂对象时最推荐的方式。

# `break` 与 `continue`

```cpp
#include <iostream>

int main() {
    for (int i = 0; i < 10; ++i) {
        if (i == 3) {
            continue;
        }

        if (i == 7) {
            break;
        }

        std::cout << i << std::endl;
    }

    return 0;
}
```

# 新旧写法

旧的容器遍历常常是这样：

```cpp
std::vector<int>::iterator it;
for (it = values.begin(); it != values.end(); ++it) {
    std::cout << *it << std::endl;
}
```

`C++11` 里通常可以写成：

```cpp
for (auto value : values) {
    std::cout << value << std::endl;
}
```

当不需要迭代器本身时，范围 `for` 的表达力和可读性都更好。

# 综合示例

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{1, 2, 3, 4, 5};

    for (auto& n : nums) {
        n *= 10;
    }

    for (const auto& n : nums) {
        std::cout << n << std::endl;
    }

    return 0;
}
```

# 推荐实践

- 数值循环优先传统 `for`。
- 遍历容器且不关心索引时，优先范围 `for`。
- 只读遍历复杂对象时优先 `const auto&`。
- 需要修改容器元素时使用 `auto&`。
- 不要在循环体中堆积过长逻辑，必要时提炼成函数。

# 小结

`C++11` 让“遍历容器”这件事从模板和迭代器细节中解放出来。范围 `for` 虽然只是一个语法增强，但它显著影响了现代 `C++` 代码的整体风格和可读性。

## 为什么循环在现代 C++ 风格里变化很大

如果你以前接触的是旧 `C++` 教程，会发现大量遍历都围绕：

- 下标
- 传统 `for`
- 迭代器样板

而在 `C++11` 项目里，你更常见的默认风格通常是：

- 数值计数用传统 `for`
- 容器遍历优先范围 `for`
- 只读遍历优先 `const auto&`

这会让代码更像“在表达业务动作”，而不是“在手工展开容器机制”。

## 值遍历、引用遍历、常量引用遍历一定要分清

这是循环章节最值得反复练习的地方。

```cpp
for (auto value : values) {
}
```

表示复制元素。

```cpp
for (auto& value : values) {
}
```

表示直接操作原元素。

```cpp
for (const auto& value : values) {
}
```

表示只读访问且避免复制。

对容器元素是 `std::string`、自定义对象等较大类型时，这三者差别非常实际。

## 什么时候仍然应该使用传统 `for`

范围 `for` 并不是万能替代，下面这些场景传统 `for` 仍然常见：

- 需要索引
- 需要跳步
- 需要同时访问前后元素
- 需要控制遍历起止位置

所以更准确的理解应该是：`C++11` 不是废掉旧循环，而是让“容器遍历”有了更好的默认写法。

## 常见误区

### 误区 1：范围 `for` 一定不会复制

不对。`auto value` 仍然是按值复制。

### 误区 2：看到 `auto&` 就总比 `auto` 更好

不对。你必须看是否真的需要修改原元素。

### 误区 3：范围 `for` 可以替代所有循环

不对。需要索引和复杂控制时，传统 `for` 仍然很合适。

## 补充建议

- 容器只读遍历优先 `const auto&`。
- 需要修改元素时再用 `auto&`。
- 简单值类型且明确需要拷贝时才用 `auto`。
- 不要让循环体承担过多职责，必要时提取函数。
