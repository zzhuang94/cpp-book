# std::size_t

`std::size_t` 是 `C++` 里一个很常见、但初学时经常“见过很多次却没真正搞懂”的类型。

你几乎一定已经在这些地方见过它：

- `sizeof(...)`
- `std::string::size()`
- `std::vector::size()`
- `std::array::size()`
- 数组长度、内存大小、下标相关代码

它不是某个“高级库里的冷门类型”，而是标准库里专门用来表达**对象大小、字节数、容量、长度、索引**的一种无符号整数类型。

----

# 它是什么

可以先记一句最重要的话：

> `std::size_t` 是一种**足够表示任意对象大小**的无符号整数类型。

这句话里有两个关键词：

- 它是整数类型
- 它是无符号类型

例如：

```cpp
#include <cstddef>
#include <iostream>

int main() {
    std::size_t n = 10;
    std::cout << n << std::endl;
    return 0;
}
```

这里的 `n` 表示“数量”或“大小”就很自然，因为这类值本来就不应该是负数。

----

# 它为什么存在

很多人第一反应会是：  
“长度不就是一个整数吗？为什么不用 `int`？”

问题在于，`int` 并不保证能表示所有平台上可能出现的对象大小。

而 `std::size_t` 的设计目的，就是专门用来承载：

- 对象所占的字节数
- 容器里元素个数
- 数组下标或长度
- 内存相关接口中的大小参数

也就是说，它不是“随便选了一个整数类型”，而是标准库在表达“大小”这件事时的正式类型。

----

# 它来自哪里

通常你会在 `#include <cstddef>` 后使用它：

```cpp
#include <cstddef>
```

当然，在很多其他头文件里你也会间接遇到它，比如：

- `<cstring>`
- `<string>`
- `<vector>`
- `<array>`

因为这些库本身就经常处理大小、长度和容量。

----

# 它和 sizeof 的关系

理解 `std::size_t`，最直接的入口就是 `sizeof`。

```cpp
#include <cstddef>
#include <iostream>

int main() {
    int x = 0;
    std::size_t n = sizeof(x);
    std::cout << n << std::endl;
    return 0;
}
```

这里 `sizeof(x)` 的结果类型就是 `std::size_t`。

这很合理，因为 `sizeof` 表达的是“某个对象占多少字节”，而这正是 `std::size_t` 最典型的职责。

如果你把这个判断建立起来，后面再看到容器的 `size()`、字符串的 `length()` 返回类似类型时，就不会觉得突兀了。

----

# 它不是固定的某一种具体类型

`std::size_t` 是一个**别名类型**，但它到底等价于哪种具体整数类型，和平台、编译器实现有关。

例如，在不同环境下，它可能表现得像：

- `unsigned int`
- `unsigned long`
- `unsigned long long`

所以你应该记住的是它的**语义**，而不是死记“它就是某个固定类型”。

真正重要的是：

- 它用来表示大小
- 它是无符号的
- 它足够大，能表示对象大小

----

# 为什么容器的 size() 常返回它

先看一个常见例子：

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{1, 2, 3, 4};
    std::size_t n = nums.size();
    std::cout << n << std::endl;
    return 0;
}
```

`std::vector::size()` 返回的通常就是一个和 `std::size_t` 相关的无符号大小类型。

原因很直接：

- 容器元素个数不应该是负数
- 容器大小本质上就是“可计数的数量”
- 这个值应该能适应平台可表示的对象规模

所以从语义上看，`size()` 返回 `std::size_t` 这一类类型，比返回 `int` 更合理。

----

# 最常见的用法

## 表示数组长度

```cpp
#include <cstddef>
#include <iostream>

int main() {
    int nums[] = {10, 20, 30, 40};
    std::size_t len = sizeof(nums) / sizeof(nums[0]);

    std::cout << len << std::endl;
    return 0;
}
```

这里 `len` 表示元素个数，用 `std::size_t` 非常自然。

## 作为下标

```cpp
#include <cstddef>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{1, 2, 3};

    for (std::size_t i = 0; i < nums.size(); ++i) {
        std::cout << nums[i] << std::endl;
    }

    return 0;
}
```

这里 `i` 和 `nums.size()` 的类型语义一致，代码更协调。

----

# 它为什么容易踩坑

`std::size_t` 本身没有问题，真正容易出错的是：  
**你一边把它当“大小类型”用，一边又忘了它是无符号数。**

这会带来两个高频坑。

----

# 坑一：不能表示负数

```cpp
#include <cstddef>
#include <iostream>

int main() {
    std::size_t n = 3;
    // n = -1;  // 不推荐这样写
    std::cout << n << std::endl;
    return 0;
}
```

如果你把负数赋给无符号类型，结果通常不会是你直觉里的“负数”，而会变成一个很大的正整数值。

这类代码之所以危险，不是因为语法错，而是因为它**看起来能编译，通过后却可能隐藏逻辑错误**。

所以一个很重要的边界意识是：

- `std::size_t` 适合表达“不会为负”的数量
- 如果你的值有可能小于 `0`，就要警惕是否选错了类型

----

# 坑二：倒序循环容易出错

这是 `std::size_t` 最经典的坑之一。

很多人会自然地写出：

```cpp
for (std::size_t i = n - 1; i >= 0; --i) {
    std::cout << i << std::endl;
}
```

但这段代码有问题，因为 `i` 是无符号数，条件 `i >= 0` 永远成立。

当 `i` 减到 `0` 之后再继续 `--i`，它不会变成负数，而会发生回绕，变成一个非常大的值。

更安全的写法通常是：

```cpp
for (std::size_t i = n; i > 0; --i) {
    std::cout << i - 1 << std::endl;
}
```

或者，如果你的逻辑天然需要“可能减到负数再停止”，那就要认真考虑是否该使用有符号类型。

----

# 和 int 混用时要小心

再看一个很常见的写法：

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{1, 2, 3};

    for (int i = 0; i < nums.size(); ++i) {
        std::cout << nums[i] << std::endl;
    }

    return 0;
}
```

这段代码在很多小例子里能正常工作，但它隐藏了一个类型混用问题：

- `i` 是 `int`
- `nums.size()` 往往是无符号大小类型

编译器经常会对此发出“有符号/无符号比较”的警告。

这类警告不只是风格问题，它提醒你：  
两边表达的“数值世界”并不完全一样。

更统一的写法是：

```cpp
for (std::size_t i = 0; i < nums.size(); ++i) {
    std::cout << nums[i] << std::endl;
}
```

当然，更现代一点时，你甚至可以直接避免手写下标循环：

```cpp
for (auto value : nums) {
    std::cout << value << std::endl;
}
```

如果你并不真的需要下标，范围 `for` 往往更简单，也更不容易犯错。

----

# 那什么时候不该用它

这也是理解 `std::size_t` 的关键。

不要因为它“常见”，就把它误以为是“默认整数类型”。

它适合这些场景：

- 长度
- 个数
- 容量
- 字节数
- 不会出现负值的下标

但它不适合这些场景：

- 需要表示 `-1`
- 需要表示“失败”或“未找到”的负值状态
- 需要做可能变成负数的差值运算

比如两个位置做差时，结果可能是负数，这时就不该盲目继续用 `std::size_t`。

----

# 和 ptrdiff_t 的区别

如果说 `std::size_t` 更偏向“大小”，那么 `std::ptrdiff_t` 更偏向“差值”。

可以先这么理解：

- `std::size_t`：表示大小、长度、容量
- `std::ptrdiff_t`：表示两个指针或位置之间的差值

因为“差值”可能为负，所以 `std::ptrdiff_t` 是有符号整数类型。

这两个类型经常一起出现，刚好说明一件事：

- “大小”通常不该是负数
- “差值”则很可能需要正负号

----

# 一个更完整的例子

```cpp
#include <cstddef>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{10, 20, 30, 40, 50};

    std::size_t total = nums.size();
    std::cout << "size = " << total << std::endl;

    for (std::size_t i = 0; i < nums.size(); ++i) {
        std::cout << "nums[" << i << "] = " << nums[i] << std::endl;
    }

    std::cout << "reverse:" << std::endl;
    for (std::size_t i = nums.size(); i > 0; --i) {
        std::cout << nums[i - 1] << std::endl;
    }

    return 0;
}
```

这个例子里可以顺手观察出三点：

- `size()` 的结果适合放进 `std::size_t`
- 正序下标循环很自然
- 倒序时不能写成 `i >= 0`

----

# 推荐实践

- 把 `std::size_t` 理解成“大小类型”，不要把它当成普通 `int` 的替代品。
- 接 `sizeof` 结果时，优先使用 `std::size_t`。
- 接容器 `size()`、`length()`、`capacity()` 这类结果时，优先使用对应的大小类型语义。
- 需要下标且不会出现负值时，使用 `std::size_t` 很自然。
- 写倒序循环时，特别注意无符号回绕问题。
- 如果逻辑里需要负数，先停一下，重新判断它是不是还适合用 `std::size_t`。
- 如果只是遍历容器而不需要下标，优先考虑范围 `for`。

----

# 小结

`std::size_t` 真正重要的不是“它到底等于哪种 unsigned 类型”，而是你要建立对它的语义判断：

- 它表示大小、长度、容量、字节数
- 它是无符号整数
- 它非常适合接 `sizeof` 和容器大小
- 它也因此会带来无符号数的典型陷阱

只要把这几个判断建立起来，你以后看到 `size()`、`sizeof`、数组长度、下标循环时，就更容易理解为什么很多现代 `C++` 代码喜欢使用 `std::size_t`，也更容易避免那些“代码能编译，但逻辑已经歪了”的隐蔽错误。