# std::size_t

在 C++11 里，`std::size_t` 依然是标准库中表达“大小”的基础类型。  
它本质上是一个**无符号整数类型**，专门用来表示对象大小、字节数、元素个数这类不会为负的值。

很多初学者第一次接触它，通常是在下面这些地方：

- `sizeof(...)`
- `std::string::size()`
- `std::vector::size()`
- 数组长度和下标相关代码

如果只记一句话，可以记成：

> `std::size_t` 是 C++11 中用来表示“大小”和“数量”的标准无符号整数类型。

---

# 背景

为什么标准库不直接用 `int` 表示长度和大小？

因为 `int` 只是一种普通整数类型，它并不保证一定能表示平台上所有对象的大小；而 `std::size_t` 的设计目标，就是**足够表示任意对象的大小**。

这也是为什么：

- `sizeof` 的结果类型是 `std::size_t`
- 标准库容器的 `size()` 通常返回与 `std::size_t` 相近的大小类型

在 C++11 里，这一点尤其值得建立成习惯：  
看到“长度、容量、字节数、元素个数”，优先想到 `std::size_t`，而不是直接写 `int`。

---

# 常见使用场景

## 1. 接收 `sizeof` 的结果

```cpp
#include <cstddef>
#include <iostream>

int main() {
    std::size_t n = sizeof(int);
    std::cout << n << std::endl;
}
```

`sizeof` 表示对象占多少字节，它的结果天然适合交给 `std::size_t`。

## 2. 保存容器大小

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{1, 2, 3};
    std::size_t n = nums.size();
    std::cout << n << std::endl;
}
```

容器元素个数不会是负数，因此用 `std::size_t` 比较自然。

## 3. 作为下标使用

```cpp
#include <cstddef>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{10, 20, 30};

    for (std::size_t i = 0; i < nums.size(); ++i) {
        std::cout << nums[i] << std::endl;
    }
}
```

当下标本身不可能为负时，`std::size_t` 和 `size()` 的语义是一致的。

---

# 注意事项

`std::size_t` 最大的特点是：**它是无符号类型**。  
优点是很适合表示大小，缺点是写错时不容易第一眼发现。

## 1. 不能表示负数

如果你的变量可能出现 `-1`、失败状态、差值为负等情况，就不应该优先选 `std::size_t`。

## 2. 倒序循环要小心

下面这种写法是有问题的：

```cpp
for (std::size_t i = n - 1; i >= 0; --i) {
    // ...
}
```

因为 `i` 是无符号数，`i >= 0` 永远成立。更安全的写法通常是：

```cpp
for (std::size_t i = n; i > 0; --i) {
    std::cout << i - 1 << std::endl;
}
```

## 3. 和 `int` 混用时容易出现警告

```cpp
for (int i = 0; i < nums.size(); ++i) {
    // ...
}
```

这里 `i` 是有符号整数，而 `nums.size()` 往往是无符号大小类型，编译器常会给出有符号/无符号比较警告。  
在 C++11 代码里，这类警告最好不要忽略。

---

# 和 int 对比

可以把两者的区别先简单记成这样：

| 类型 | 语义 | 是否可为负 | 典型用途 |
| --- | --- | --- | --- |
| `std::size_t` | 大小、长度、容量、字节数 | 否 | `sizeof`、`size()`、下标 |
| `int` | 普通整数 | 是 | 计数、运算结果、可能出现负值的场景 |

使用时可以遵循一个简单判断：

- 表示“大小”时，优先考虑 `std::size_t`
- 表示“普通整数”时，继续使用 `int`
- 如果结果可能为负，不要为了“统一风格”硬用 `std::size_t`

也就是说，`std::size_t` 不是 `int` 的升级版，它只是更适合“大小”这个语义。

---

# 小结

在 C++11 背景下，`std::size_t` 最重要的不是“它到底等于哪种 unsigned 类型”，而是它表达的含义：

- 它表示大小和数量
- 它适合接 `sizeof` 和容器 `size()`
- 它作为下标很常见
- 它是无符号类型，因此要特别注意负数和倒序循环问题

把它理解成“大小类型”，就不容易和 `int` 混用了。