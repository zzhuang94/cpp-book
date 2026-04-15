# pair

## 概述

> `std::pair<T1, T2>` 就是一个“装两个值”的小结构体：  
它有两个公开成员 `first` / `second`，常用来表示**二元组**、**键值对**、或者函数返回“两个结果”。

最常见的出现场景：

- 三元及以上异质打包见 [tuple](tuple.md)；
- `map` / `unordered_map` 的元素类型就是 `std::pair<const Key, T>`；
- 算法里常把“值 + 下标/来源”打包（比如最小值与位置）；
- 函数想返回两个值时，不想为了这点东西专门写一个结构体；

需要注意的几个点：

- `pair` 的比较是**字典序（lexicographical order）**：先比 `first`，相等再比 `second`。
- `std::make_pair(...)` 会做类型推导；但遇到引用/数组/字符串字面量时，推导结果可能与你想的不一样（例如 `"hi"` 会推成 `const char*`）。

----

## 接口

| 成员 | 功能与用法 |
|------|------------|
| `std::pair<T1,T2> p;` | 默认构造（两个成员会各自默认构造）。 |
| `std::pair<T1,T2> p(a,b);` | 用两个值构造。 |
| `std::pair<T1,T2> p{a,b};` | 列表初始化（更常用）。 |
| `p.first` / `p.second` | 两个公开成员。 |
| `p = {a,b}` | 赋值为新的两个值。 |
| `p1 == p2` / `!=` | 两个成员都相等才相等。 |
| `p1 < p2` / `<=` / `>` / `>=` | 字典序比较：先比 `first`，再比 `second`。 |
| `std::make_pair(a,b)` | 返回一个推导类型的 `pair`（会发生衰减/复制，注意引用场景）。 |
| `std::tie(a,b)` | 返回 `tuple` 的引用绑定，常用于“解包赋值”或比较/忽略字段（可配合 `std::ignore`）。 |
| `auto [x,y] = p;` | C++17 结构化绑定（默认是拷贝；想绑定引用用 `auto& [x,y] = p;`）。 |

----

## 示例

```cpp
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <utility>

static void print_pair(const std::pair<int, std::string>& p, const char* title) {
    std::cout << title << " (" << p.first << ", " << p.second << ")\n";
}

int main() {
    // ---------- 基本构造 / CTAD / make_pair ----------
    std::pair<int, std::string> p1{1, "one"};
    std::pair p2(2, std::string("two")); // C++17：CTAD 推导出 pair<int,string>
    auto p3 = std::make_pair(3, "three"); // 注意：这里 second 推导为 const char*

    print_pair(p1, "p1=");
    print_pair(p2, "p2=");
    std::cout << "p3=(" << p3.first << ", " << p3.second << ")\n";

    // ---------- 比较：字典序 ----------
    std::pair<int, int> a{1, 9};
    std::pair<int, int> b{2, 0};
    std::pair<int, int> c{1, 10};
    std::cout << std::boolalpha;
    std::cout << "(1,9) < (2,0) ? " << (a < b) << "\n";   // true：先比 first
    std::cout << "(1,9) < (1,10) ? " << (a < c) << "\n"; // true：first 相等再比 second

    // ---------- 结构化绑定：拷贝 vs 引用 ----------
    std::pair<int, int> pos{10, 20};
    auto [x, y] = pos; // x/y 是拷贝
    x = 99;
    std::cout << "after copy bind, pos.first=" << pos.first << "\n"; // 仍然是 10

    auto& [rx, ry] = pos; // rx/ry 绑定到原对象
    rx = 77;
    std::cout << "after ref bind, pos.first=" << pos.first << "\n"; // 变成 77

    // ---------- tie：解包赋值 / ignore ----------
    std::pair<int, std::string> user{42, "alice"};
    int id = 0;
    std::string name;
    std::tie(id, name) = user;
    std::cout << "id=" << id << " name=" << name << "\n";

    // 只想要 second：first 忽略
    std::tie(std::ignore, name) = user;
    std::cout << "name=" << name << "\n";

    // ---------- pair 在 map 里：元素就是 pair<const K, V> ----------
    std::map<std::string, int> cnt;
    ++cnt["apple"];
    ++cnt["apple"];
    ++cnt["banana"];

    for (const auto& kv : cnt) {
        // kv 的类型是 pair<const std::string, int>
        std::cout << kv.first << " -> " << kv.second << "\n";
    }

    return 0;
}
```
