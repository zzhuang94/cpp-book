# forward_list

## 概述

> `forward_list` 是「瘦身版 `list`」：只有向前的指针，省一点内存，接口却更绕

- 插入删除多在 **`insert_after` / `erase_after`** 上完成，还得习惯 **`before_begin()`** 这个「首元素之前的哨兵」。
- 为了省空间，甚至**不要求**实现维护 `size()`，想知道长度就自己数或额外维护计数。
- 没有 `push_back`：尾插要一路走到尾，成本摆在那里。

若你老要在某个元素**前面**插、又不想维护前驱指针，不如换 `list` 省心

----

## 接口

| 成员 | 功能与用法 |
|------|------------|
| `forward_list<T> fl` | 空单向链表。 |
| `fl.before_begin()` | 首元素之前的迭代器，用于 `insert_after` 等。 |
| `fl.push_front(x)` / `fl.emplace_front(args...)` | 头插。 |
| `fl.pop_front()` | 删首元素（空表未定义）。 |
| `fl.insert_after(it, x)` / `emplace_after(it, args...)` | 在 `it` **之后**插入。 |
| `fl.erase_after(it)` | 删 `it` 后面那一个节点。 |
| `fl.begin()` / `fl.end()` | 正向迭代（非双向）。 |
| `fl.empty()` / `fl.clear()` / `fl.swap` | 无 `size()`。 |
| `fl.remove(val)` / `remove_if` / `sort` / `unique` | 成员算法。 |

----

## 示例

```cpp
#include <forward_list>
#include <iostream>

int main() {
    std::forward_list<int> fl = {1, 2, 3, 4, 5};

    // 在表头前插入：在 before_begin 之后插入
    std::forward_list<int>::iterator before = fl.before_begin();
    fl.insert_after(before, 0);

    std::cout << "after head insert:";
    for (int x : fl) {
        std::cout << ' ' << x;
    }
    std::cout << "\n";

    // 删掉首元之后的节点（即原来的第一个元素）
    if (!fl.empty()) {
        fl.erase_after(fl.before_begin());
    }

    std::cout << "after erase_after before_begin:";
    for (int x : fl) {
        std::cout << ' ' << x;
    }
    std::cout << "\n";

    // 删除所有值为 2 的节点：prev + cur 双指针
    std::forward_list<int>::iterator prev = fl.before_begin();
    std::forward_list<int>::iterator cur = fl.begin();
    while (cur != fl.end()) {
        if (*cur == 2) {
            cur = fl.erase_after(prev);
        } else {
            prev = cur;
            ++cur;
        }
    }

    std::cout << "remove all 2:";
    for (int x : fl) {
        std::cout << ' ' << x;
    }
    std::cout << "\n";

    // 在第一个元素之后插入 99
    std::forward_list<int>::iterator first = fl.begin();
    if (first != fl.end()) {
        fl.insert_after(first, 99);
    }

    std::cout << "after insert_after first:";
    for (int x : fl) {
        std::cout << ' ' << x;
    }
    std::cout << "\n";

    fl.sort();
    std::cout << "sorted:";
    for (int x : fl) {
        std::cout << ' ' << x;
    }
    std::cout << "\n";

    return 0;
}
```
