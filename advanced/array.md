# 数组

数组是 `C++` 里非常基础的话题，但现代 `C++` 对数组的态度已经很明确：裸数组仍然存在，但在绝大多数工程代码里，应该优先考虑 `std::array` 和 `std::vector`。

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

# `std::array`

`C++11` 引入了 `std::array`，它是固定大小数组的现代替代方案：

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

# 初始化

由于早期标准细节和聚合初始化规则，`std::array` 在 `C++11` 中常看到双层花括号：

```cpp
std::array<int, 3> nums{{1, 2, 3}};
```

这是一种很典型的 `C++11` 风格，看到时不要觉得奇怪。

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

# 范围 `for`

`C++11` 范围 `for` 与数组非常搭配：

```cpp
int nums[] = {1, 2, 3, 4};

for (auto n : nums) {
    std::cout << n << std::endl;
}
```

如果使用 `std::array`，效果更好，因为类型和接口更完整。

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

# 与 `std::vector` 的区别

这虽然不完全是“数组”主题，但工程里必须对比清楚：

- 裸数组：固定大小，接口弱
- `std::array`：固定大小，类型安全，接口完整
- `std::vector`：动态大小，最常用

# 综合示例

```cpp
#include <array>
#include <iostream>
#include <numeric>

int main() {
    std::array<int, 5> nums{{1, 2, 3, 4, 5}};
    int sum = std::accumulate(nums.begin(), nums.end(), 0);

    std::cout << "sum = " << sum << std::endl;

    for (const auto& n : nums) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

# 推荐实践

- 固定长度数据优先使用 `std::array`。
- 动态长度数据优先使用 `std::vector`。
- 只有在底层接口、性能约束或历史代码需要时才直接使用裸数组。
- 遍历数组时优先范围 `for`。
- 需要边界检查时使用 `at()`。

# 小结

数组相关最重要的迁移思路不是“会不会写 `int a[10]`”，而是知道什么时候应该停止使用裸数组。`C++11` 提供的 `std::array` 和更顺滑的标准库协作方式，已经足以覆盖很多旧式数组用法。

## 为什么数组在 C++ 里是迁移思维的重要分水岭

对很多其它语言开发者来说，“数组”天然就应该是一个带长度、能安全遍历、最好还能动态扩容的容器。但在 `C++` 里：

- 裸数组只是很基础的语言设施
- 它不会自动携带长度
- 它会在很多场景退化为指针
- 它本身和标准库接口协作能力有限

也正因此，现代 `C++` 才会强调：

- 固定长度优先 `std::array`
- 动态长度优先 `std::vector`

## 数组退化成指针这件事一定要有印象

例如函数参数：

```cpp
void print(int nums[]) {
}
```

它并不真的保留数组长度信息，很多时候更接近：

```cpp
void print(int* nums) {
}
```

这正是很多旧式代码容易埋坑的地方之一。

## `std::array` 的真正价值

它的价值不只是“写法新一点”，而是：

- 长度在类型里
- 支持 `size()`、`at()`、迭代器
- 能自然配合范围 `for` 和算法库

这会让固定长度数据结构从“半底层语法”升级为“正常标准库对象”。

## 什么时候该选 `vector`

如果你发现自己在问这些问题：

- 元素个数运行时才能确定吗
- 需要插入、扩容吗
- 需要和算法频繁组合吗

那大概率已经更适合 `std::vector` 了，而不是继续执着于裸数组。

## 常见误区

### 误区 1：数组章节只讲最基础语法

不对。这里其实在帮你建立“什么时候不用裸数组”的现代判断。

### 误区 2：`std::array` 和裸数组差不多

不对。它在接口完整性和标准库协作上好很多。

### 误区 3：固定长度也该默认上 `vector`

不一定。固定长度场景下，`std::array` 常常更直接。

## 补充建议

- 固定长度优先 `std::array`。
- 动态长度优先 `std::vector`。
- 只有底层接口或历史代码场景才主动使用裸数组。
- 一看到数组参数就警惕长度信息是否丢失。
- 遍历数组时优先范围 `for`，减少边界错误。
