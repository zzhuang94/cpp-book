# unordered_set

## 1 概述

`unordered_set` 只存键、不存值，底层哈希，**无序**、平均常数时间查找——可以理解为「去重集合的哈希版」。经典用法：`insert` 完看一眼返回的 `pair` 里 `second` 是不是 `true`，就知道是不是第一次见到这个元素。需要键值对请用 `unordered-map.md`；要排序遍历用 `set.md`。自定义类型时，和 `unordered_map` 一样要准备 **哈希** 与 **`==`**（或自定义 `KeyEqual`）。`rehash`、`reserve` 也可能让迭代器一锅端掉。

头文件 `<unordered_set>`。接口比 `unordered_map` 少一维映射，其它习惯相近。

----

## 2 接口

| 成员 | 功能与用法 |
|------|------------|
| `unordered_set<K> s` | 空集。 |
| `s.insert(x)` / `s.emplace(args...)` | 插入；返回 `pair<iterator,bool>` 可知是否新插入。 |
| `s.erase(it)` / `s.erase(k)` | 删除。 |
| `s.find(k)` / `s.count(k)` | 查找、计数（0 或 1）。 |
| `s.reserve` / `s.rehash` | 控制 rehash，减少扩容次数。 |
| `s.bucket_count` / `load_factor` 等 | 观察桶与负载。 |
| `s.begin` / `s.end` / `empty` / `size` / `clear` / `swap` | 常规。 |

----

## 3 示例

```cpp
#include <functional>
#include <iostream>
#include <unordered_set>

struct Point {
    int x;
    int y;

    bool operator==(const Point& o) const {
        if (x != o.x) {
            return false;
        }
        if (y != o.y) {
            return false;
        }
        return true;
    }
};

namespace std {

template <>
struct hash<Point> {
    std::size_t operator()(const Point& p) const {
        const long long mix =
            (static_cast<long long>(p.x) << 32) ^ static_cast<unsigned int>(p.y);
        return std::hash<long long>()(mix);
    }
};

}  // namespace std

int main() {
    std::unordered_set<Point> pts;
    pts.reserve(16);

    Point a = {1, 2};
    Point b = {3, 4};
    Point c = {1, 2};

    std::pair<std::unordered_set<Point>::iterator, bool> r1 = pts.insert(a);
    std::pair<std::unordered_set<Point>::iterator, bool> r2 = pts.insert(b);
    std::pair<std::unordered_set<Point>::iterator, bool> r3 = pts.insert(c);

    if (r1.second) {
        std::cout << "inserted new point\n";
    }
    if (!r3.second) {
        std::cout << "duplicate point not inserted again\n";
    }

    std::cout << "size=" << pts.size() << "\n";

    if (pts.find(b) != pts.end()) {
        std::cout << "has (3,4)\n";
    }

    if (pts.erase(b) > 0) {
        std::cout << "erased (3,4)\n";
    }

    std::cout << "final size=" << pts.size() << "\n";

    return 0;
}
```
