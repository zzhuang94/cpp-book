# unordered_map

## 1 概述

`unordered_map` 是 C++11 的哈希表版字典：平均意义上查找、插入、删除都很快，但**遍历出来不按键排序**，和 `map.md` 那种「打印永远整整齐齐」的感觉完全不同。键要能算哈希（默认 `std::hash<K>`）还要能判等。做大表缓存、词频、图邻接表都很常见。`reserve` 能减少 rehash；可一旦 **rehash**，迭代器可能**全体作废**，别在插入循环里攥着旧迭代器不放。`[]` 和 `map` 一样会悄悄插入缺失键——只读查询请用 `find` 或 `at`。

头文件 `<unordered_map>`。可自定义 `Hash` 与 `KeyEqual`。最坏情况是大家全挤进一个桶，复杂度会退化，线上数据分布要心里有数。

----

## 2 接口

| 成员 | 功能与用法 |
|------|------------|
| `unordered_map<K,V> m` | 空表。 |
| `m[k]` / `m.at(k)` | 访问；`[]` 可插入，`at` 不存在抛异常。 |
| `m.insert(x)` / `m.emplace(args...)` | 插入键值对。 |
| `m.erase(it)` / `m.erase(k)` / 区间 `erase` | 删除。 |
| `m.find(k)` / `m.count(k)` | 查找、计数（0 或 1）。 |
| `m.reserve(n)` | 预留元素规模，减少 rehash（C++11）。 |
| `m.rehash(n)` | 请求桶数不少于实现相关阈值（C++11）。 |
| `m.bucket_count()` / `m.load_factor()` / `m.max_load_factor()` | 观察与调节负载。 |
| `m.begin()` / `m.end()` | 本地迭代器（遍历无序）。 |
| `m.empty()` / `m.size()` / `m.clear()` / `m.swap` | 常规。 |

----

## 3 示例

```cpp
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

int main() {
    std::unordered_map<std::string, int> wc;
    wc.reserve(32);

    wc.emplace("apple", 2);
    wc.emplace("banana", 5);
    wc["apple"] += 1;

    if (wc.find("banana") != wc.end()) {
        std::cout << "banana count=" << wc["banana"] << "\n";
    }

    try {
        int v = wc.at("missing");
        (void)v;
    } catch (const std::out_of_range& ex) {
        std::cout << "at missing: " << ex.what() << "\n";
    }

    const std::string text = "abracadabra";
    std::unordered_map<char, int> freq;
    for (std::size_t i = 0; i < text.size(); ++i) {
        char ch = text[i];
        freq[ch] += 1;
    }

    std::cout << "freq a=" << freq['a'] << " b=" << freq['b'] << "\n";

    for (std::unordered_map<char, int>::iterator it = freq.begin(); it != freq.end(); ) {
        if (it->second >= 3) {
            it = freq.erase(it);
        } else {
            ++it;
        }
    }

    std::cout << "after erase high freq:";
    for (std::unordered_map<char, int>::const_iterator j = freq.begin(); j != freq.end(); ++j) {
        std::cout << ' ' << j->first << ':' << j->second;
    }
    std::cout << "\n";

    std::cout << "buckets=" << wc.bucket_count() << " load=" << wc.load_factor() << "\n";

    return 0;
}
```
