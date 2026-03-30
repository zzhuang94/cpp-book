# 容器、算法、迭代器

现代 `C++` 代码里，这三者经常是一起出现的。
- **容器** 负责存放和组织数据
- **算法** 负责对一段数据做通用处理
- **迭代器** 负责把容器和算法连接起来

你看到一个 `vector`，往往不是只关心“里面装了什么”，还要继续看“后面配什么算法”“算法操作的是哪段区间”。

----

# 容器

> 容器的职责很简单：把一批数据管理起来，并提供统一的访问方式。

不同容器的区别，不只是“底层结构不同”，更重要的是：
- 元素是否连续存储
- 查找方式是什么
- 是否有顺序
- 是否允许重复
- 是否适合和某些算法搭配

先建立一个基本认识：**容器不是孤立存在的，它真正的价值通常体现在“后面准备怎么处理这些数据”。**

## 动态数组 std::vector

它是最常见、也最值得优先考虑的容器。

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

它的直觉可以先记成这样：
- 连续内存
- 随机访问快
- 末尾插入通常高效
- 和大多数标准算法配合自然

> 如果你暂时不知道该选什么容器，先想到 `vector` 往往是最稳妥的起点。

很多“就是存一批元素，然后遍历、查找、排序、过滤”的场景，最后都会落到它身上。

## 双端队列 std::deque

如果你需要两端都比较频繁地插入和删除，可以考虑 `deque`。

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

它不像 `vector` 那样强调连续内存，但在双端操作场景里更合适。

## 双向链表 std::list

`list` 是双向链表。它在某些 *中间位置频繁插入删除* 的场景有优势，但在现代工程里，通常没有 `vector` 那么常见。

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

> 容器不是越“底层结构感强”就越高级。工程里更常见的是先选最常用、最顺手、最容易和标准库协作的方案。

## 关联容器

有些场景不是“存一串值”，而是“按 key 管理数据”或者“保证元素唯一”。这时常见的是下面几类容器。

### std::map

`map` 是按 key 排序的关联容器：

```cpp
#include <iostream>
#include <map>
#include <string>

int main() {
    std::map<std::string, int> scores;
    scores["alice"] = 90;
    scores["charlie"] = 88;
    scores["bob"] = 85;

    for (const auto& item : scores) {
        std::cout << item.first << ": " << item.second << std::endl;
    }
    return 0;
}
// 输出
// alice: 90
// bob: 85
// charlie: 88
```

### std::unordered_map

如果你更关心按 key 快速查找，而不关心遍历顺序，`unordered_map` 往往更像直觉上的第一选择。

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

### std::set

如果你要的是“元素唯一”，而且希望按顺序组织，可以考虑 `set`。

```cpp
#include <iostream>
#include <set>

int main() {
    std::set<int> values{4, 3, 1, 2, 2};

    for (auto v : values) {
        std::cout << v << " ";
    }
    std::cout << std::endl;
    return 0;
}
// 输出 1 2 3 4
```


----

# 算法

> 标准库算法的价值在于：你不必每次都重新手写循环。

排序、查找、计数、复制、删除条件匹配元素，这些动作本来就很通用，所以标准库把它们做成了独立算法。

例如排序：

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

再看一个和 `lambda` 配合的例子：

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

这就是 `C++11` 之后非常典型的标准库风格：

- 容器存数据
- 算法处理数据
- `lambda` 描述条件或规则

> 顺手建立一个阅读习惯：看到容器时，不要只盯着“里面装了什么”，还要继续看“后面打算配什么算法一起用”。

----

# 迭代器

想把标准库真正看顺，迟早要接受一个事实：**很多算法并不是直接操作容器，而是操作迭代器。**

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{3, 1, 2, 4};
    auto it = std::find(nums.begin(), nums.end(), 2);

    if (it != nums.end()) {
        std::cout << *it << std::endl;
    }
    return 0;
}
```

你可以先把迭代器理解成 `容器给算法提供的统一访问入口`。这也是为什么很多标准库算法都长得像：

```cpp
algorithm(begin, end, ...);
```

它们不急着知道容器的具体类型，而是先通过区间来工作。

## 操作区间

这里最值得先记住的是：`[begin, end)` 表示 **左闭右开区间**。

- `begin` 指向第一个元素
- `end` 不是最后一个元素
- **`end` 指向的是“最后一个元素的下一个位置”**

这也是为什么很多算法判断“没找到”时，返回的都是 `end()`：

```cpp
auto it = std::find(nums.begin(), nums.end(), 100);
if (it == nums.end()) {
    std::cout << "not found" << std::endl;
}
```

一旦接受了这个规则，很多接口都会突然变得统一起来。
比如“遍历整个容器”“查找一段区间”“复制一段内容”，本质上都只是在处理一个 `[begin, end)`。

## 返回迭代器

算法返回迭代器，而不是直接返回下标或元素本身，是因为它想尽量通用。

- 有的容器本来就没有“下标”这个概念，例如 `list`
- 返回元素值会丢失“位置”信息
- 返回迭代器后，调用者还能继续在那个位置上操作

例如：

```cpp
auto it = std::find(nums.begin(), nums.end(), 2);
if (it != nums.end()) {
    *it = 20;
}
```

> 这类写法背后的意思是：算法只负责把“位置”交给你，至于后面要读、改、删，决定权留给调用方。

## 迭代器能力

迭代器虽然提供了统一接口，但不同容器给出的“能力”并不完全一样。

- `vector` 的迭代器支持随机访问
- `list` 的迭代器通常只能双向移动
- 有些算法要求更强的迭代器能力

这会直接影响算法能不能用。例如下面这种写法对 `vector` 很自然：

```cpp
std::sort(nums.begin(), nums.end());
```

但如果换成 `list`，就不能直接这样写，因为 `std::sort` 需要随机访问迭代器，而 `list` 不提供这种能力。

所以读标准库代码时，除了问“这是什么容器”，还可以继续多问一句：**它给算法提供的是哪一类迭代器能力？**

----

# 容器怎么选

把容器、算法、迭代器放在一起看之后，选容器时的判断会更稳定：

- 大多数情况下先考虑 `vector`
- 需要按 key 管理数据时考虑 `map` / `unordered_map`
- 需要元素唯一时考虑 `set`
- 需要双端高效操作时考虑 `deque`
- 只有在确实适合时再考虑 `list`

> 不要在没有证据时因为“理论复杂度”过早选择冷门容器。

更实际的问法通常是：

- 我主要是顺序遍历，还是按 key 查找
- 我后面会不会大量排序、查找、过滤
- 我需不需要随机访问
- 我是否依赖元素唯一或有序

----

# 失效意识

容器章节还有一个很容易在项目里踩坑的点：**你保存下来的引用、指针或迭代器，不一定会一直有效。**

例如对 `vector` 来说，如果后续插入元素导致扩容，之前拿到的旧地址、旧引用、旧迭代器就可能失效。

所以一旦你打算长期保存这些“指向容器内部元素的东西”，就应该顺手问自己：

- 后面还会继续插入吗
- 容器会不会重新分配内存
- 这份引用、指针或迭代器现在是不是已经不安全了

这个意识之所以重要，正是因为容器、算法、迭代器经常混在一起使用。一旦位置失效，后续算法代码也可能一起出问题。

----

# 小结

这一章真正要建立的，不是“我记住了多少容器名字”，而是“我能不能把容器、算法、迭代器当成一套配合关系来理解”。

- 容器负责存数据
- 算法负责处理数据
- 迭代器负责描述区间和位置
- 选容器时优先考虑后续算法怎么配合
- 保存引用、指针、迭代器时顺手考虑失效问题

只要把这条主线抓住，后面再学习更多标准库内容时，就不会只看到零碎 API，而会看到它们之间的组织方式。
