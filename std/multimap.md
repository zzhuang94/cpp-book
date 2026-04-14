# multimap

## 1 概述

`multimap` 和 `map` 听起来像双胞胎，差别却只有一句：**同一个键可以出现很多次**。电话簿里一个人两条号码、日志里同一用户连刷多条记录，这种一对多用 `map` 得自己再套 `vector`；用 `multimap`，标准库会把相同键的元素排在一起，`equal_range` 一拉就是一整段。它没有 `operator[]`——一个键底下挂着好几条值时，`m[k]` 根本说不清该读写哪一条，标准干脆禁掉了。

元素仍是 `pair<const K, V>`，排序只看键。删的时候要留神：`erase(k)` 会**一锅端**删掉所有这个键；若只想动其中一条，先用 `find` 拿迭代器再 `erase(it)`。若键必须唯一，请回到 `map.md`。更高标准里还有哈希版的多重映射；在 C++11 语境下，你至少先记牢：它**有序**，适合既要一对多又要按键扫的场景。

----

## 2 接口

头文件 `<map>`。

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

## 3 示例

```cpp
#include <iostream>
#include <map>
#include <string>

int main() {
    std::multimap<std::string, std::string> phone;

    phone.emplace("Alice", "111-1111");
    phone.emplace("Alice", "222-2222");
    phone.emplace("Bob", "333-3333");

    std::cout << "Alice lines:\n";
    std::pair<std::multimap<std::string, std::string>::iterator,
              std::multimap<std::string, std::string>::iterator>
        rng = phone.equal_range("Alice");
    for (std::multimap<std::string, std::string>::iterator it = rng.first; it != rng.second; ++it) {
        std::cout << "  " << it->second << "\n";
    }

    std::multimap<std::string, std::string>::iterator f = phone.find("Bob");
    if (f != phone.end()) {
        std::cout << "Bob one entry: " << f->second << "\n";
    }

    std::cout << "count Alice=" << phone.count("Alice") << "\n";

    return 0;
}
```
