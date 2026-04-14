# map

## 概述

> `std::map` 是最常用的「键 → 值」容器：像一本按关键字排序的字典，`每个键只出现一次`。

业务里做配置表、计数器、索引，第一反应常常就是它。  
底层一般是平衡树，查找、插入、删除都是 O(log n) 量级，**键排序遍历**，支持键区间裁剪。

- 若你 **不在乎顺序、只要大表平均更快**，可以看 `unordered-map`;
- 若一个键底下要挂 **多条** 记录，就该换 `multimap`。

元素类型是 **`std::pair<const K, V>`** ：键带 `const`，防止改键把整棵树顺序弄崩。

!> **特别需要注意：** `map` 的 `m[k]` 在键不存在时会 *顺手插入* 一个默认构造的值。

----

## 接口

| 成员 | 功能与用法 |
|------|------------|
| `map<K,V> m` | 默认构造空表。 |
| `map<K,V,Compare> m` | 用 `Compare` 排序键，默认 `std::less<K>`。 |
| `m[k]` | 访问或插入：无则插入 `V()` 再返回引用。 |
| `m.at(k)` | 访问：无键抛 `std::out_of_range`（C++11）。 |
| `m.insert(x)` | 插入 `value_type`；返回 `pair<iterator,bool>`，`bool` 表示是否新插入。 |
| `m.emplace(args...)` | 原地构造 `pair`（C++11）。 |
| `m.emplace_hint(hint, args...)` | 带位置提示的 `emplace`（C++11）。 |
| `m.erase(it)` / `m.erase(first,last)` | 按迭代器删除。 |
| `m.erase(k)` | 按键删，返回删除条数（0 或 1）。 |
| `m.find(k)` | 找键，返回迭代器，找不到为 `end()`。 |
| `m.count(k)` | 返回 0 或 1（C++11 起对 map 等价于是否在）。 |
| `m.lower_bound(k)` | 第一个 `>= k` 的键。 |
| `m.upper_bound(k)` | 第一个 `> k` 的键。 |
| `m.equal_range(k)` | `pair<iterator,iterator>`，等价于 `lower` 与 `upper`。 |
| `m.begin()` / `m.end()` | 迭代器，遍历为键升序。 |
| `m.empty()` / `m.size()` / `m.clear()` | 判空、元素个数、清空。 |
| `m.swap(other)` | 交换两表（常数时间交换内部结构）。 |
| `m.key_comp()` / `m.value_comp()` | 返回比较函数对象。 |

----

## 示例

```cpp
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

int main() {
    std::map<std::string, int> score;

    // operator[]：写入或读已存在键
    score["Alice"] = 90;
    score["Bob"] = 85;

    if (score.find("Carol") == score.end()) {
        std::cout << "Carol not in table (find does not insert)\n";
    }

    // insert：已存在则不覆盖
    std::pair<std::map<std::string, int>::iterator, bool> ins1 =
        score.insert(std::make_pair("Bob", 0));
    if (!ins1.second) {
        std::cout << "Bob already there, value still " << ins1.first->second << "\n";
    }

    score.emplace("Dave", 88);

    // at：键必须存在
    try {
        int a = score.at("Alice");
        std::cout << "Alice score=" << a << "\n";
        (void)score.at("Nobody");
    } catch (const std::out_of_range& ex) {
        std::cout << "at() missing key: " << ex.what() << "\n";
    }

    // 遍历：按键序
    for (std::map<std::string, int>::const_iterator it = score.begin(); it != score.end(); ++it) {
        std::cout << it->first << " => " << it->second << "\n";
    }

    // lower_bound / upper_bound
    std::map<int, char> rank;
    rank[10] = 'a';
    rank[20] = 'b';
    rank[30] = 'c';

    std::map<int, char>::iterator lo = rank.lower_bound(15);
    std::map<int, char>::iterator hi = rank.upper_bound(25);
    std::cout << "keys in (15,25] by iterators:\n";
    for (std::map<int, char>::iterator j = lo; j != hi; ++j) {
        std::cout << "  " << j->first << " -> " << j->second << "\n";
    }

    // 删除
    if (score.erase("Bob") > 0) {
        std::cout << "Bob erased\n";
    }

    return 0;
}
```
