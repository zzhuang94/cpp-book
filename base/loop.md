# 循环

循环结构在 `C++98` 与 `C++11` 之间看起来变化不大，但 `C++11` 增加的范围 `for` 显著改变了容器遍历的默认写法。

----

# while

适合“循环次数不固定，只要条件满足就继续”的场景。

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

----

# do...while

特点是循环体至少执行一次。

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

----

# 传统 for

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

----

# 范围 for

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

> 这通常是遍历复杂对象时最推荐的方式。

----

# break 与 continue

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

----

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

----

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

循环这一章真正需要建立的判断，是“我到底在做计数，还是在做遍历”。
一旦把这个问题想清楚，传统 `for`、范围 `for`、`auto`、`auto&`、`const auto&` 的选择就会自然很多。

----

# 推荐实践

- 数值循环优先传统 `for`。
- 遍历容器且不关心索引时，优先范围 `for`。
- 只读遍历复杂对象时优先 `const auto&`。
- 需要修改容器元素时使用 `auto&`。
- 不要在循环体中堆积过长逻辑，必要时提炼成函数。

----

# 小结

- `C++11` 没有废掉旧循环，而是让“遍历容器”这件事有了更自然的默认写法。
- 只要你分清楚自己需要的是索引、复制、修改还是只读访问，循环写法就不会乱。
