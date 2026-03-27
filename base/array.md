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
