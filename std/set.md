# set

## 1 概述

可以把 `std::set` 想成「只关心键、不关心值」的 `map`：里面是一堆**互不相同**的元素，从小到大排好队。适合去重、判断出现过没有、又希望遍历结果**有序**的场景。需要键值对请看 `map.md`；允许重复键用 `multiset.md`；不要顺序、只要哈希的平均速度，用 `unordered-set.md`。

插入重复键时，`insert` 会返回 `pair<iterator,bool>` 告诉你「有没有真的插进去」——没插进去说明原来就有。比较器照样得是**严格弱序**，和 `map` 一条绳上的蚂蚱。`erase(k)` 可以传键，返回值是删掉了几条（0 或 1）。C++11 起同样有 `emplace`、移动和 `cbegin`/`cend`；迭代器解引用是 `const Key&`，没有 `.second`。若你发现自己其实在维护「键 → 值」两段信息，就别硬把 `pair` 塞进 `set` 当键了，换 `map` 可读性会好很多。

----

## 2 接口

头文件 `<set>`。

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

## 3 示例

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
    for (std::set<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << "\n";

    if (names.erase("bob") > 0) {
        std::cout << "removed bob\n";
    }

    std::set<int> nums;
    nums.insert(5);
    nums.insert(1);
    nums.insert(3);
    std::set<int>::iterator lb = nums.lower_bound(3);
    if (lb != nums.end()) {
        std::cout << "lower_bound(3) points to " << *lb << "\n";
    }

    return 0;
}
```
