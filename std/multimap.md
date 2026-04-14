# multimap

## 概述

> `multimap` 和 `map` 听起来像双胞胎，差别却只有一句：**同一个键可以出现很多次**。

- 电话簿里一个人两条号码、日志里同一用户连刷多条记录，这种一对多用 `map` 得自己再套 `vector`；
- 用 `multimap`，标准库会把相同键的元素排在一起，`equal_range` 一拉就是一整段。
- 它没有 `operator[]`，一个键底下挂着好几条值时，`m[k]` 根本说不清该读写哪一条，标准干脆禁掉了。

> 元素仍是 `pair<const K, V>`，排序只看键
- 删的时候要留神：`erase(k)` 会**一锅端**删掉所有这个键；
- 若只想动其中一条，先用 `find` 拿迭代器再 `erase(it)`。

若键必须唯一，请回到 `map.md`。更高标准里还有哈希版的多重映射；

在 C++11 语境下，你至少先记牢：它**有序**，适合既要一对多又要按键扫的场景。

----

## 接口

| 成员 | 功能与用法 |
|------|------------|
| `multimap<K,V> mm` | 空表。 |
| `mm.insert(x)` / `mm.emplace(args...)` | 插入，允许重复键。 |
| `mm.erase(it)` / `mm.erase(first,last)` | 按迭代器删。 |
| `mm.erase(k)` | 删除键等于 `k` 的**全部**元素，返回删除个数。 |
| `mm.find(k)` | 找到任一匹配键的迭代器；无则 `end()`。 |
| `mm.count(k)` | 键 `k` 的出现次数。 |
| `mm.equal_range(k)` | `pair<it,it>`，覆盖所有键为 `k` 的元素。 |
| `mm.lower_bound` / `mm.upper_bound` | 同有序关联容器语义。 |
| `mm.begin` / `mm.end` / `mm.empty` / `mm.size` / `mm.clear` / `mm.swap` | 与 `map` 类似。 |

----

## 示例

```cpp
#include <iostream>
#include <map>
#include <string>

int main() {
    // multimap：一个 key 可以对应多个 value（与 map 不同，map 的 key 唯一）。
    std::multimap<std::string, std::string> phone;

    // emplace 直接在容器内构造元素；这里故意插入重复的 "Alice" 来体现 multimap 的特点。
    phone.emplace("Alice", "111-1111");
    phone.emplace("Alice", "222-2222");
    phone.emplace("Bob", "333-3333");

    std::cout << "Alice lines:\n";
    // equal_range(key) 返回一个半开区间 [first, second)：
    // - first 指向该 key 的第一条记录
    // - second 指向“该 key 的最后一条记录的下一个位置”（也就是第一个 key 不再等于它的位置）
    auto rng = phone.equal_range("Alice");
    for (auto it = rng.first; it != rng.second; ++it) {
        std::cout << "  " << it->second << "\n";
    }

    // find(key) 只返回“某一条”匹配记录的迭代器（通常是该 key 的第一条，但标准不要求你依赖具体哪一条）。
    // 如果你需要遍历该 key 的全部记录，用 equal_range 更合适。
    auto f = phone.find("Bob");
    if (f != phone.end()) {
        std::cout << "Bob one entry: " << f->second << "\n";
    }

    // count(key) 返回该 key 出现的次数；对 multimap 来说可能大于 1。
    std::cout << "count Alice=" << phone.count("Alice") << "\n";

    return 0;
}
```
