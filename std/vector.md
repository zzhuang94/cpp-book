# vector

## 概述

> `std::vector` 大概是 C++ 里被用得最多的容器：  
一块能**自动变长**的连续内存，按下标访问和 C 数组一样直白，却多了 `push_back`、`size()` 这些省心接口。

- 和 `array` 里那种编译期定长数组相比，`vector` 把长度交给运行期；
- 和 `deque`、`list` 相比，它在 **尾部追加** 时更贴合 CPU 缓存，但 **头插、中间大段插入** 就会痛——那是换容器的信号。

扩容时可能整块搬家，于是 **迭代器、指针、引用** 可能一瞬间全部作废；  
长循环里 `push_back` 前顺手 `reserve` 一下，往往少踩一半坑。

`vector<int>(3)` 和 `vector<int>{3}` 根本不是一回事：
- `(3)` 表示创建一个大小为 3 的 vector
- `{3}` 表示创建一个只含 1 个元素的 vector，这个元素的值是 3

`v[i]` 不检查越界，真要查边界用 `at`。别把 `vector` 的 `[]` 和 `map` 里会「自动插入」的 `[]` 搞混了。

----

## 接口

| 成员 | 功能与用法 |
|------|------------|
| `vector<T> v` / `v(n)` / `v(n,x)` | 默认空；或 n 个默认构造；或 n 个值为 x。 |
| `v{...}` / `v = {...}` | 初始化列表构造（注意与 `v(n)` 区分）。 |
| `v(begin,end)` | 迭代器区间拷贝构造。 |
| `v.assign(...)` | 用 n 个值、或迭代器区间、或 `initializer_list` 整体替换内容。 |
| `v.size()` / `v.empty()` / `v.clear()` | 元素个数、是否为空、清空（不保证 `capacity` 变 0）。 |
| `v.capacity()` / `v.reserve(n)` | 当前容量、预分配至少容纳 n 个元素的空间。 |
| `v.shrink_to_fit()` | C++11：请求释放多余容量（实现可忽略）。 |
| `v.resize(n)` / `v.resize(n,val)` | 改变元素个数，多出来的用默认或 `val` 填。 |
| `v.push_back(x)` | 先在外面 `造好一个对象`，然后把它​​拷贝（或移动）​​到 vector 里面 |
| `v.emplace_back(args...)` | 把 `对象的原材料（参数）` 传进去，在 vector 内部​​原地组装，**性能更好** |
| `v.pop_back()` | 删最后一个（空则未定义）。 |
| `v[i]` / `v.at(i)` / `v.front()` / `v.back()` | 访问；`at` 越界抛 `std::out_of_range`。 |
| `v.data()` | C++11：首元素指针；空 `vector` 勿解引用。 |
| `v.begin()` / `v.end()` / `cbegin` / `cend` | 迭代器（C++11 `cbegin`/`cend`）。 |
| `v.insert(it,x)` / `emplace(it,args...)` | 在 `it` 前插入。 |
| `v.erase(it)` / `v.erase(first,last)` | 删除元素或区间，返回下一迭代器。 |
| `v.swap(other)` | 交换两容器内容（常数时间交换内部指针）。 |

----

## 示例

```cpp
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

struct Item {
    int id;
    std::string name;

    Item(int i, std::string n) : id(i), name(std::move(n)) {
    }
};

static void print_int_vec(const std::vector<int>& v, const char* title) {
    std::cout << title << " size=" << v.size() << " :";
    for (std::size_t i = 0; i < v.size(); ++i) {
        std::cout << ' ' << v[i];
    }
    std::cout << "\n";
}

int main() {
    // ---------- 初始化：空、重复初值、列表、圆括号与花括号陷阱 ----------
    std::vector<int> empty_one;
    std::vector<int> three_default(3);
    std::vector<int> three_sevens(3, 7);
    std::vector<int> list_style{10, 11, 12};
    std::vector<int> parens_means_count(3);
    std::vector<int> braces_means_one_elem{3};

    if (empty_one.empty()) {
        std::cout << "empty_one is empty\n";
    }

    std::cout << "three_default first=" << three_default[0] << "\n";
    std::cout << "three_sevens: ";
    for (int x : three_sevens) {
        std::cout << x << ' ';
    }
    std::cout << "\n";

    std::cout << "list_style size=" << list_style.size() << "\n";
    std::cout << "parens(3) size=" << parens_means_count.size() << "\n";
    std::cout << "{3} size=" << braces_means_one_elem.size()
              << " value=" << braces_means_one_elem[0] << "\n";

    // ---------- 容量与尾部增长 ----------
    std::vector<int> work;
    work.reserve(64);
    for (int i = 0; i < 16; ++i) {
        work.push_back(i * i);
    }
    std::cout << "after loop size=" << work.size()
              << " capacity>=" << work.capacity() << "\n";

    std::vector<Item> items;
    items.emplace_back(1, "apple");
    items.push_back(Item(2, "banana"));
    for (std::size_t i = 0; i < items.size(); ++i) {
        std::cout << "item " << items[i].id << " " << items[i].name << "\n";
    }

    // ---------- 访问：front/back/at ----------
    if (!work.empty()) {
        std::cout << "front=" << work.front() << " back=" << work.back() << "\n";
    }

    try {
        (void)work.at(9999);
    } catch (const std::out_of_range& ex) {
        std::cout << "at() threw as expected\n";
    }

    // ---------- 插入、按条件删除（迭代器与 erase 返回值）----------
    work.insert(work.begin() + 1, 99);
    print_int_vec(work, "after insert 99 at pos 1");

    for (std::vector<int>::iterator it = work.begin(); it != work.end(); ) {
        if (*it == 99) {
            it = work.erase(it);
        } else {
            ++it;
        }
    }
    print_int_vec(work, "after erase 99");

    work.push_back(2);
    work.push_back(2);
    work.push_back(3);
    print_int_vec(work, "before erase-remove all 2");
    // std::remove 把容器中等于某个特定值的元素“移到”末尾
    // 但它绝对不会真正删除元素，也不会改变容器的大小！​
    // 相似的还有 std::remove_if
    work.erase(std::remove(work.begin(), work.end(), 2), work.end());
    print_int_vec(work, "after erase-remove all 2");

    // ---------- resize / assign / swap ----------
    work.resize(6);
    work.resize(10, -1);
    print_int_vec(work, "after resize");

    std::vector<int> other;
    other.assign(4u, 42);
    print_int_vec(other, "other assign");

    work.swap(other);
    print_int_vec(work, "after swap work");
    print_int_vec(other, "after swap other");

    // ---------- data() 与二维 vector ----------
    if (!other.empty()) {
        int* raw = other.data();
        if (raw != nullptr) {
            raw[0] = 1000;
        }
    }
    print_int_vec(other, "after touch data()");

    std::vector<std::vector<int>> mat(2, std::vector<int>(3, 1));
    if (mat.size() >= 2) {
        mat[1][2] = 9;
    }
    for (std::size_t r = 0; r < mat.size(); ++r) {
        std::cout << "row " << r << ":";
        for (std::size_t c = 0; c < mat[r].size(); ++c) {
            std::cout << ' ' << mat[r][c];
        }
        std::cout << "\n";
    }

    // ---------- clear / shrink_to_fit；移动赋值 ----------
    other.clear();
    other.shrink_to_fit();

    std::vector<int> donor;
    for (int k = 0; k < 5; ++k) {
        donor.push_back(k);
    }
    other = std::move(donor);
    std::cout << "after move-assign, donor.size=" << donor.size()
              << " other.size=" << other.size() << "\n";

    return 0;
}
```
