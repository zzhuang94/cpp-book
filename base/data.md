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
