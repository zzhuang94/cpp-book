# 数据结构

这里的“数据结构”主要指标准库中最常用的容器与配套算法。接手 `C++11` 项目时，你几乎不可能绕开 `STL`。如果还停留在“手写链表、数组加下标循环”的思路，阅读现代代码会非常吃力。

# 标准库容器

因为标准库容器已经帮你处理了大量底层细节：

- 内存管理
- 大小扩缩
- 迭代接口
- 与算法库协作

这意味着你应该尽量把精力放在“选择合适的数据结构”，而不是重复造轮子。

# `std::vector`

最常用的动态数组容器：

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{1, 2, 3};
    nums.push_back(4);

    for (auto n : nums) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

特点：

- 连续内存
- 随机访问快
- 末尾插入通常高效

# `std::list`

双向链表：

```cpp
#include <iostream>
#include <list>

int main() {
    std::list<int> values{1, 2, 3};
    values.push_front(0);

    for (auto v : values) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

它在特定插入删除场景有优势，但在现代工程中通常没有 `vector` 使用频率高。

# `std::deque`

双端队列，支持两端高效插入删除：

```cpp
#include <deque>
#include <iostream>

int main() {
    std::deque<int> q{1, 2, 3};
    q.push_front(0);
    q.push_back(4);

    for (auto v : q) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

# `std::map`

按键排序的关联容器：

```cpp
#include <iostream>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> scores;
    scores["alice"] = 90;
    scores["bob"] = 85;

    for (const auto& item : scores) {
        std::cout << item.first << ": " << item.second << std::endl;
    }
    return 0;
}
```

# `std::unordered_map`

`C++11` 新增的重要容器之一，基于哈希表：

```cpp
#include <iostream>
#include <string>
#include <unordered_map>

int main() {
    std::unordered_map<std::string, int> ages;
    ages["alice"] = 20;
    ages["bob"] = 25;

    std::cout << ages["alice"] << std::endl;
    return 0;
}
```

这是 `C++11` 项目里非常常见的选择。

# `std::set`

存放不重复元素的有序集合：

```cpp
#include <iostream>
#include <set>

int main() {
    std::set<int> values{3, 1, 2, 2};

    for (auto v : values) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

# 算法配合

容器的真正威力常常来自与算法库组合使用：

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{5, 1, 4, 3, 2};
    std::sort(nums.begin(), nums.end());

    for (auto n : nums) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

再看一个 `lambda` 例子：

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{1, 2, 3, 4, 5};
    auto count = std::count_if(nums.begin(), nums.end(), [](int x) {
        return x % 2 == 0;
    });

    std::cout << count << std::endl;
    return 0;
}
```

这正是 `C++11` 之后标准库写法的典型风格。

# 容器选择

简单经验：

- 大多数情况下先考虑 `vector`
- 需要按 key 查找时考虑 `map` / `unordered_map`
- 需要元素唯一时考虑 `set`
- 需要双端高效操作时考虑 `deque`

不要在没有证据时因为“理论复杂度”过早选择冷门容器。

# 小结

现代 `C++` 的数据结构学习重点，不是手写各种经典结构，而是熟练掌握标准库容器与算法。`C++11` 通过范围 `for`、`auto`、`lambda` 和无序容器，让这一套组合式写法变得比过去顺手得多。

## 为什么这章是进入真实项目的高频章节

在现代 `C++` 项目里，你几乎每天都会遇到：

- `vector`
- `map`
- `unordered_map`
- `set`
- 迭代器
- `sort`、`find_if`、`count_if`

所以这章不是“附带的标准库知识”，而是日常业务代码的主战场。

## 先把容器选择思路建立起来

最实用的第一原则可以很简单：

- 默认先想到 `vector`
- 键值查找再考虑 `map` / `unordered_map`
- 需要有序唯一集合再考虑 `set`
- 只有明确需要双端高效操作时再考虑 `deque`

很多初学者容易一上来就被容器家族吓住，但实际上大部分项目代码集中在少数几个容器上。

## 迭代器是理解标准库的桥梁

你可以先把迭代器理解成“容器与算法之间的统一访问接口”。这也是为什么：

```cpp
std::sort(nums.begin(), nums.end());
```

这种写法在 `C++` 里如此常见。算法不必知道你是 `vector` 还是别的容器，它主要通过迭代器工作。

## 容器失效规则值得尽早建立印象

例如 `vector` 扩容后，旧迭代器、旧引用、旧指针可能失效；这不是冷知识，而是真实项目里经常出现的问题来源。

所以你在保存容器元素地址、引用或迭代器时，要主动考虑：

- 后续是否会插入新元素
- 容器是否可能扩容
- 这个引用是否还能继续安全使用

## 算法库的思维方式

很多其它语言开发者进入 `C++` 后，一开始会继续习惯手写循环。但现代 `C++` 非常鼓励把“遍历 + 条件 + 动作”交给算法库和 `lambda` 组合表达。

例如：

```cpp
auto count = std::count_if(nums.begin(), nums.end(), [](int x) {
    return x % 2 == 0;
});
```

这种风格一旦习惯了，代码可读性会明显提升。

## `vector` 为什么通常是默认首选

虽然理论上每个容器都有擅长场景，但在很多工程代码里，`vector` 之所以高频，是因为它同时兼顾了：

- 连续内存
- 良好的缓存友好性
- 足够丰富的标准库支持
- 足够直观的接口

很多旧式“链表一定很高级”的直觉，在现代实践里并不成立。

## 常见误区

### 误区 1：学数据结构就是手写各种底层结构

不对。项目里更重要的是会选和会用标准库容器。

### 误区 2：知道容器名字就够了

不对。失效规则、迭代器和算法协作同样重要。

### 误区 3：理论复杂度最优就一定是最佳选择

不对。工程里还要考虑常数、内存布局、可读性和使用频率。

## 补充建议

- 默认先考虑 `vector`。
- 理解迭代器是算法库接口的核心。
- 保存容器元素地址、引用、迭代器前先想失效规则。
- 尽量学会用算法库表达常见遍历逻辑。
- 没有明确理由时，不要为了“理论复杂度”过早选冷门容器。
