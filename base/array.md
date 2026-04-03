# 数组

数组是 `C++` 里非常基础的话题，但现代 `C++` 对数组的态度已经很明确：
> 裸数组仍然存在，但在绝大多数工程代码里，应该优先考虑 `std::array` 和 `std::vector`。

----

# C 风格数组

```cpp
#include <iostream>

int main() {
    int nums[3] = {1, 2, 3};
    for (int i = 0; i < 3; ++i) {
        std::cout << nums[i] << std::endl;
    }
    return 0;
}
```

这种写法简单直接，但问题也很明显：

- 大小固定
- 容易退化为指针
- 不记录长度
- 缺少统一接口

----

# std::array

`C++11` 引入了 `std::array`，它是 **固定大小数组** 的现代替代方案：

```cpp
#include <array>
#include <iostream>

int main() {
    std::array<int, 3> nums{{1, 2, 3}};
    for (auto value : nums) {
        std::cout << value << std::endl;
    }
    return 0;
}
```

它的优点：

- 大小固定但类型完整
- 支持 `size()`、`at()`、迭代器
- 能更自然地与标准库算法协作

它和裸数组最关键的区别，不只是“多几个成员函数”，而是你终于拿到一个真正的标准库对象，而不只是语言层面的底层数组语法。

----

# 初始化

由于早期标准细节和聚合初始化规则，`std::array` 在 `C++11` 中常看到双层花括号：

```cpp
std::array<int, 3> nums{{1, 2, 3}};
```

这是一种很典型的 `C++11` 风格，看到时不要觉得奇怪。

----

# 访问元素

```cpp
#include <array>
#include <iostream>

int main() {
    std::array<int, 3> nums{{10, 20, 30}};

    std::cout << nums[0] << std::endl;
    std::cout << nums.at(1) << std::endl;
    std::cout << nums.size() << std::endl;
    return 0;
}
```

和字符串章节类似，这里也可以顺手建立一个边界意识：

- `operator[]` 更直接，但不做边界检查
- `at()` 更安全，越界时会抛出异常

调试、学习或者边界不够确定时，`at()` 往往更容易帮你发现问题。

----

# 范围 for

`C++11` 范围 `for` 与数组非常搭配：

```cpp
int nums[] = {1, 2, 3, 4};

for (auto n : nums) {
    std::cout << n << std::endl;
}
```

如果使用 `std::array`，效果更好，因为类型和接口更完整。

----

# 多维数组

```cpp
#include <iostream>

int main() {
    int matrix[2][3] = {
        {1, 2, 3},
        {4, 5, 6}
    };

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
```

如果维度复杂或需要动态大小，通常应考虑 `std::vector`。

----

# 作为函数参数

数组这一章最容易被忽略、但在真实项目里很重要的一点，是“数组传进函数之后会发生什么”。

## 旧式数组

```cpp
#include <iostream>

void print(int nums[], int size) {
    for (int i = 0; i < size; ++i) {
        std::cout << nums[i] << " ";
    }
    std::cout << std::endl;
}
```

!> 这里的 `int nums[]` 在函数参数位置并不会帮你保留数组长度信息，很多时候它更接近：

```cpp
void print(int* nums, int size) {
}
```

这也是为什么旧式数组代码经常还要额外传一个长度参数。

## 现代化数组

如果你用的是 `std::array`，接口表达通常会更完整：

```cpp
#include <array>
#include <iostream>

void print(const std::array<int, 3>& nums) {
    for (auto n : nums) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
}
```

----

# 对比 std::vector

这虽然不完全是“数组”主题，但工程里必须对比清楚：

- 裸数组：固定大小，接口弱
- `std::array`：固定大小，类型安全，接口完整
- `std::vector`：动态大小，最常用

可以先用一个很朴素的判断来选：

- 元素个数编译期就确定，优先想 `std::array`
- 元素个数运行时才知道，优先想 `std::vector`
- 只有在底层接口或历史代码里，才继续直接写裸数组

----

# 综合示例

```cpp
#include <array>
#include <iostream>
#include <numeric>

int main() {
    std::array<int, 5> nums{{1, 2, 3, 4, 5}};

    // std::accumulate 是 C++ 标准库 <numeric> 头文件中提供的一个数值累加算法
    int sum = std::accumulate(nums.begin(), nums.end(), 0);

    std::cout << "sum = " << sum << std::endl;

    for (const auto& n : nums) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

----

# 推荐实践

- 固定长度数据优先使用 `std::array`。
- 动态长度数据优先使用 `std::vector`。
- 只有在底层接口、性能约束或历史代码需要时才直接使用裸数组。
- 遍历数组时优先范围 `for`。
- 需要边界检查时使用 `at()`。

----

# 小结

数组这一章真正要建立的判断，是“这里需要的是底层数组语法，还是一个更完整的容器对象”。

- 固定长度优先 `std::array`
- 动态长度优先 `std::vector`
- *看到裸数组参数时，先警惕长度信息是否已经丢失*

只要先建立这个选择顺序，很多旧式数组代码为什么容易出错、现代代码为什么更偏向容器，也就更容易看懂了。
