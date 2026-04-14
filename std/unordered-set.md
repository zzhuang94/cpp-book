# unordered_set

## 概述

> `unordered_set` 只存键、不存值，底层哈希，**无序**、平均常数时间查找——可以理解为「去重集合的哈希版」。

经典用法：`insert` 完看一眼返回的 `pair` 里 `second` 是不是 `true`，就知道是不是第一次见到这个元素。

----

## 接口

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

## 示例

```cpp
#include <functional>
#include <iostream>
#include <unordered_set>

// 一个用在 unordered_set 里的自定义键类型。
// 关键点：unordered_set 判重依赖两件事：
// - “相等”：operator== 或自定义 KeyEqual
// - “哈希”：std::hash<T> 或自定义 Hash
struct Point {
    int x;
    int y;

    // 定义“两个 Point 是否代表同一个键”。
    // unordered_set 会先用哈希把元素分桶，再用 == 在桶内做精确判等。
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

// 为用户自定义类型 Point 提供 std::hash 特化，这是标准允许的用法。
// 有了这个特化，就能直接写 unordered_set<Point>，无需额外传 Hash 模板参数。
template <>
struct hash<Point> {
    std::size_t operator()(const Point& p) const {
        // 把 (x, y) 混合成一个 64 位值再哈希。
        // 这里把 x 放到高 32 位，把 y 放到低 32 位（以位运算的方式“拼起来”）。
        const long long mix =
            (static_cast<long long>(p.x) << 32) ^ static_cast<unsigned int>(p.y);
        return std::hash<long long>()(mix);
    }
};

}  // namespace std

int main() {
    std::unordered_set<Point> pts;
    // 预留桶/容量，减少插入时的 rehash 次数（示例里不是必须，但有助于性能习惯）。
    pts.reserve(16);

    Point a = {1, 2};
    Point b = {3, 4};
    Point c = {1, 2};

    // insert 返回 pair<iterator, bool>：
    // - iterator 指向集合中“最终存在的那个元素”（新插入的或已存在的）
    // - bool 表示这次是否真的插入了新元素（true=新插入，false=重复未插入）
    auto r1 = pts.insert(a);
    auto r2 = pts.insert(b);
    auto r3 = pts.insert(c);

    if (r1.second) {
        std::cout << "inserted new point\n";
    }
    if (!r3.second) {
        std::cout << "duplicate point not inserted again\n";
    }

    std::cout << "size=" << pts.size() << "\n";

    // find：存在则返回迭代器，否则返回 end()。
    if (pts.find(b) != pts.end()) {
        std::cout << "has (3,4)\n";
    }

    // erase(key) 返回删除的元素个数。
    // 对 unordered_set 来说，要么删 1 个，要么删 0 个。
    if (pts.erase(b) > 0) {
        std::cout << "erased (3,4)\n";
    }

    std::cout << "final size=" << pts.size() << "\n";

    return 0;
}
```
