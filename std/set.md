# set

## 概述

> 可以把 `std::set` 想成「只关心键、不关心值」的 `map`：里面是一堆**互不相同**的元素，从小到大排好队。

- 适合去重、判断出现过没有、又希望遍历结果**有序**的场景
- 允许重复键用 `multiset`；
- 不要顺序、只要哈希的平均速度，用 `unordered-set`。

插入重复键时，`insert` 会返回 `pair<iterator,bool>` 告诉你「有没有真的插进去」——没插进去说明原来就有。

> 比较器是**严格弱序**，和 `map` 一样。

这里的「比较器」指 `set<K, Compare>` 里的 `Compare`：一个“决定谁排在谁前面”的规则，通常长这样：

```cpp
struct Compare {
    bool operator()(const K& a, const K& b) const; // a 是否应该排在 b 前面？
};
```

`set` 的默认比较器是 `std::less<K>`（一般等价于用 `<` 做升序）。`set` 的“有序”和“去重”都只看比较器，并不依赖 `==`。

所谓**严格弱序**（strict weak ordering），你可以把它理解成“像正常的排序规则一样靠谱”，至少要满足这些直觉性质：

- **不能自己比自己小**：`comp(x, x)` 必须为 `false`
- **传递性**：若 `comp(a,b)` 且 `comp(b,c)`，则必须 `comp(a,c)`
- **等价也要一致**：若 `a` 与 `b` 等价、`b` 与 `c` 等价，则 `a` 与 `c` 也必须等价

## 接口

| 成员 | 功能与用法 |
|------|------------|
| `set<K> s` | 空集。 |
| `set<K,Compare> s` | 自定义键比较。 |
| `s.insert(x)` / `s.emplace(args...)` | 插入；已存在则不变（`insert` 返回 `pair<iterator,bool>`）。 |
| `s.erase(it)` / `s.erase(first,last)` | 按迭代器删。 |
| `s.erase(k)` | 按键删，返回删除个数（0 或 1）。 |
| `s.find(k)` | 查找，无则 `end()`。 |
| `s.count(k)` | 0 或 1。 |
| `s.lower_bound` / `s.upper_bound` / `s.equal_range` | 有序区间定位，语义同 `map` 的键侧。 |
| `s.begin()` / `s.end()` | 升序遍历。 |
| `s.empty()` / `s.size()` / `s.clear()` / `s.swap` | 与其它容器类似。 |

----

## 示例

```cpp
#include <iostream>
#include <set>
#include <string>

int main() {
    std::set<std::string> names;

    names.insert("bob");
    names.insert("alice");
    names.insert("carol");
    names.insert("alice");

    if (names.count("alice") > 0) {
        std::cout << "alice in set\n";
    }

    std::cout << "sorted names:";
    for (auto it = names.begin(); it != names.end(); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << "\n";

    if (names.erase("bob") > 0) {
        std::cout << "removed bob\n";
    }
    std::cout << "after erase bob:";
    for (auto it = names.begin(); it != names.end(); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << "\n";

    std::set<int> nums;
    nums.insert(5);
    nums.insert(1);
    nums.insert(3);
    std::set<int>::iterator lb = nums.lower_bound(2);
    if (lb != nums.end()) {
        std::cout << "lower_bound(3) points to " << *lb << "\n";
    }

    return 0;
}
```
