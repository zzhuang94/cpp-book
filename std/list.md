# list

## 概述

> `std::list` 是经典的双向链表：节点散落在堆上，用指针串起来，所以没有 `[]`，访问第 n 个元素只能走迭代器。

好处也很实在：在 **已知迭代器的位置 插入或删除是 O(1)** ，而且不会像 `vector` 那样一扩容就全体迭代器作废。

`splice` 还能把另一条链表整段「接」过来，常数时间、不拷贝节点。  
反过来，如果你大部分时间只是在 **顺序扫一遍**，`list` 往往跑不过 `vector`，缓存不友好是链表的老毛病。  
需要随机下标请用 `vector` 或 `deque`；
只要单向、想再省一点指针，可以看 `forward_list`。

----

## 接口

| 成员 | 功能与用法 |
|------|------------|
| `list<T> lst` | 空链表。 |
| `lst.push_back` / `push_front` / `pop_back` / `pop_front` | 两端增删。 |
| `lst.insert(it, x)` / `lst.emplace(it, args...)` | 在 `it` 前插入（C++11 `emplace`）。 |
| `lst.erase(it)` | 删除 `it` 指向节点，返回下一位置迭代器。 |
| `lst.splice(pos, other)` | 把 `other` 整链接到 `pos` 前，`other` 变空。 |
| `lst.splice(pos, other, it)` / `...,(first,last)` | 接一条或一段。 |
| `lst.merge(other)` | 归并两条**已升序**链表，结果仍在 `lst`，`other` 清空。 |
| `lst.sort()` / `lst.unique()` | 链表专用排序、去重相邻相同。 |
| `lst.remove(val)` / `lst.remove_if(pred)` | 按值或谓词删。 |
| `lst.size()` / `empty()` / `clear()` / `swap` | 常规。 |
| `lst.begin()` / `lst.end()` | 双向迭代器。 |

----

## 示例

```cpp
#include <iostream>
#include <list>
#include <string>

int main() {
    std::list<int> a = {1, 2, 3, 4, 5};

    // 在迭代器前插入、删除
    std::list<int>::iterator it = a.begin();
    for (int step = 0; step < 2; ++step) {
        ++it;
    }
    a.insert(it, 99);

    std::cout << "after insert:";
    for (int x : a) {
        std::cout << ' ' << x;
    }
    std::cout << "\n";

    for (std::list<int>::iterator j = a.begin(); j != a.end(); ) {
        if (*j == 99) {
            j = a.erase(j);
        } else {
            ++j;
        }
    }

    // splice：把 b 整表接到 a 的某位置前，b 变空
    std::list<int> b = {10, 20, 30};
    std::list<int>::iterator pos = a.begin();
    ++pos;
    a.splice(pos, b);

    std::cout << "after splice a:";
    for (int x : a) {
        std::cout << ' ' << x;
    }
    std::cout << " | b.empty=" << (b.empty() ? "true" : "false") << "\n";

    // merge：两表须升序
    std::list<int> u = {1, 3, 5};
    std::list<int> v = {2, 4, 6};
    u.merge(v);
    std::cout << "merged u:";
    for (int x : u) {
        std::cout << ' ' << x;
    }
    std::cout << "\n";

    // emplace
    std::list<std::pair<int, std::string> > kv;
    kv.emplace_back(1, "one");
    kv.emplace_front(0, "zero");
    for (std::list<std::pair<int, std::string> >::const_iterator p = kv.begin(); p != kv.end(); ++p) {
        std::cout << p->first << '=' << p->second << ' ';
    }
    std::cout << "\n";

    return 0;
}
```
