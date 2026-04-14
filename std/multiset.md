# multiset

## 概述

> `multiset` 就是允许重复的 `set`：只有键、没有映射值，但同一个值可以出现很多遍。

做「多重集」、中位数流、带重复的排名时，它比 `set` 顺手，`count` 直接告诉你堆了几层。

----

## 接口

| 成员 | 功能与用法 |
|------|------------|
| `multiset<K> ms` | 空多重集。 |
| `ms.insert(x)` / `ms.emplace(...)` | 插入，允许与已有键相等。 |
| `ms.erase(it)` | 删迭代器指向的**一条**。 |
| `ms.erase(k)` | 删光所有等于 `k` 的元素，返回删除个数。 |
| `ms.count(k)` | 等于 `k` 的元素个数。 |
| `ms.equal_range(k)` | 所有等于 `k` 的迭代器区间。 |
| `ms.find(k)` | 指向其中一条等于 `k` 的元素。 |
| `ms.lower_bound` / `ms.upper_bound` | 有序定位。 |
| `ms.begin` / `ms.end` / `ms.size` / `ms.empty` / `ms.clear` / `ms.swap` | 常规。 |

----

## 示例

```cpp
#include <iostream>
#include <set>

int main() {
    std::multiset<int> bag;

    bag.insert(3);
    bag.insert(1);
    bag.insert(4);
    bag.insert(1);
    bag.insert(5);
    bag.insert(1);

    std::cout << "count(1)=" << bag.count(1) << "\n";

    std::cout << "sorted:";
    for (auto it = bag.begin(); it != bag.end(); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << "\n";

    auto one = bag.find(1);
    if (one != bag.end()) {
        bag.erase(one);
    }

    std::cout << "after erasing one instance of 1, count(1)=" << bag.count(1) << "\n";

    if (bag.erase(5) > 0) {
        std::cout << "erased all 5\n";
    }

    return 0;
}
```
