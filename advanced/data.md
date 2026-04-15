# 容器、算法、迭代器

现代 `C++` 代码里，这三者经常是一起出现的。
- **容器** 负责存放和组织数据
- **算法** 负责对一段数据做通用处理
- **迭代器** 负责把容器和算法连接起来

你看到一个 `vector`，往往不是只关心“里面装了什么”，还要继续看“后面配什么算法”“算法操作的是哪段区间”。

----
# 容器

> 容器的职责很简单：把一批数据管理起来，并提供统一的访问方式。

不同容器的区别，不只是“底层结构不同”，更重要的是：元素是否连续存储、查找方式、是否有序、是否允许重复、是否适合与特定算法搭配。

先建立一个基本认识：**容器不是孤立存在的，它真正的价值通常体现在“后面准备怎么处理这些数据”。**

下文各小节直接收录原 `std/*.md` 中的概述、接口表与完整示例，便于与上文总览及后文算法、迭代器章节对照阅读。

## 定长数组 `std::array`

`std::array<T, N>` 是 **C++11** 引入的定长顺序容器，头文件 `<array>`。

它像是给裸数组套了一层标准库外壳：长度 **`N` 写死在类型里**，  
既不能 `push_back`，也不会偷偷 realloc，适合 RGB、固定长度缓冲、小矩阵行这种「编译期就知道要几个」的场景。

把它嵌在结构体里当成员，往往比塞一个 `vector` 更直白，少一次堆分配的心理负担。  
代价是：`N` 不同的两个 `array` 根本不是同一个类型，没法互相赋值；

`array<T,0>` 在标准里是合法的怪胎，`front`/`back` 就别乱碰了。把元素交给只认指针的 C 函数时，用 `data()` 取首地址；若是空数组，别解引用。初始化时若类型推导犯迷糊，可以用双层花括号 `{{...}}` 和 `initializer_list` 拉开距离。


| 成员 | 功能与用法 |
|------|------------|
| `array<T,N> a` | 默认初始化 N 个元素（可能为默认构造值）。 |
| `a = { ... }` / `a = {{...}}` | 聚合初始化；双花括号可消除歧义。 |
| `a[i]` / `a.at(i)` | 下标访问；`at` 越界抛 `std::out_of_range`。 |
| `a.front()` / `a.back()` | 首尾元素（`N==0` 时未定义）。 |
| `a.data()` | C++11：指向首元素的指针。 |
| `a.begin()` / `a.end()` / `cbegin` / `cend` | 迭代器。 |
| `a.size()` / `a.max_size()` | 均为 N；`empty()` 当且仅当 `N==0` 为真。 |
| `a.fill(x)` | 每个元素赋值为 x。 |
| `a.swap(b)` | 与另一 `array<T,N>` 交换全部元素（逐元素交换）。 |
| `std::get<I>(a)` | C++11：按编译期下标 I 取元素。 |

```cpp
#include <array>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void sum_c_api(const int* p, std::size_t n, int* out) {
    int sum = 0;
    for (std::size_t i = 0; i < n; ++i) {
        sum += p[i];
    }
    *out = sum;
}

}  // namespace

struct Pixel {
    std::array<unsigned char, 3> rgb;
};

int main() {
    // ---------- 初始化与拷贝 ----------
    std::array<int, 4> a = {1, 2, 3, 4};
    std::array<int, 4> b = a;
    b[0] = 99;

    if (a[0] != b[0]) {
        std::cout << "copy is independent: a[0]=" << a[0] << " b[0]=" << b[0] << "\n";
    }

    std::array<int, 5> agg = {{10, 20, 30, 40, 50}};

    // ---------- 下标、at、范围 for ----------
    for (std::size_t i = 0; i < agg.size(); ++i) {
        std::cout << agg[i];
        if (i + 1 < agg.size()) {
            std::cout << ' ';
        }
    }
    std::cout << "\n";

    try {
        (void)agg.at(100);
    } catch (const std::out_of_range& ex) {
        std::cout << "at() caught: " << ex.what() << "\n";
    }

    // ---------- fill 与 swap ----------
    std::array<int, 3> x = {1, 2, 3};
    std::array<int, 3> y = {7, 8, 9};
    x.swap(y);
    std::cout << "after swap x[0]=" << x[0] << " y[0]=" << y[0] << "\n";

    x.fill(0);
    std::cout << "after fill x:";
    for (int v : x) {
        std::cout << ' ' << v;
    }
    std::cout << "\n";

    // ---------- data 交给 C 风格函数 ----------
    std::array<int, 4> nums = {2, 3, 4, 5};
    int summed = 0;
    if (!nums.empty()) {
        sum_c_api(nums.data(), nums.size(), &summed);
    }
    std::cout << "sum(nums)=" << summed << "\n";

    // ---------- 作结构体成员 ----------
    Pixel pix;
    pix.rgb[0] = 255;
    pix.rgb[1] = 128;
    pix.rgb[2] = 0;
    unsigned acc = 0;
    for (std::size_t i = 0; i < pix.rgb.size(); ++i) {
        acc += pix.rgb[i];
    }
    std::cout << "pixel channel sum=" << acc << "\n";

    // ---------- std::get ----------
    std::array<int, 3> tri = {5, 6, 7};
    std::cout << "get<0>=" << std::get<0>(tri) << " get<2>=" << std::get<2>(tri) << "\n";

    return 0;
}
```

## 动态数组 `std::vector`

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

## 双端队列 `std::deque`


> `deque`（读作 deck）常被叫「双端队列」：**两头 `push`、`pop` 都顺手** ，中间还能用 **`[]`** 当数组使。

- 和 `vector` 不同，它 **不保证** 一整块连续内存，所以别把 `&dq[0]` 当成能一把 `memcpy` 到底的指针；
- 和 `list` 比，它又保留了随机访问的甜头。

滑动窗口、单调队列里，它几乎是标配配角。中间插入删除要花多少代价，跟具体标准库实现有关，一般夹在 `vector` 和 `list` 之间

如果 **只在尾部** 折腾，`vector` 往往更简单；若 **两头都动** 还要下标，再把 `deque` 请上台。


| 成员 | 功能与用法 |
|------|------------|
| `deque<T> d` | 空双端队列。 |
| `d.push_back(x)` / `d.emplace_back(args...)` | 尾部插入（C++11 `emplace_back`）。 |
| `d.push_front(x)` / `d.emplace_front(args...)` | 头部插入。 |
| `d.pop_back()` / `d.pop_front()` | 删尾 / 删头（空容器调用未定义）。 |
| `d[i]` / `d.at(i)` | 随机访问；`at` 越界抛异常。 |
| `d.front()` / `d.back()` | 首尾元素引用。 |
| `d.begin()` / `d.end()` | 迭代器（随机访问迭代器类别，与 `vector` 类似）。 |
| `d.insert(it, x)` / `d.erase(it)` | 中间插入删除。 |
| `d.size()` / `d.empty()` / `d.clear()` | 大小、判空、清空。 |
| `d.swap(other)` | 交换。 |
| `shrink_to_fit()` | C++11：请求释放多余容量（实现可忽略）。 |


```cpp
#include <deque>
#include <iostream>
#include <vector>

int main() {
    std::deque<int> dq;

    // 双端入队
    for (int i = 0; i < 3; ++i) {
        dq.push_back(10 + i);
    }
    for (int j = 0; j < 2; ++j) {
        dq.push_front(j);
    }

    std::cout << "size=" << dq.size() << " front=" << dq.front() << " back=" << dq.back() << "\n";

    // 下标遍历
    for (std::size_t i = 0; i < dq.size(); ++i) {
        std::cout << "dq[" << i << "]=" << dq[i];
        if (i + 1 < dq.size()) {
            std::cout << ' ';
        }
    }
    std::cout << "\n";

    // 双端弹出
    if (!dq.empty()) {
        dq.pop_front();
    }
    if (!dq.empty()) {
        dq.pop_back();
    }

    std::cout << "after pops:";
    for (int x : dq) {
        std::cout << ' ' << x;
    }
    std::cout << "\n";

    // 与 vector 结合：滑动窗口最大值（单调 deque 存下标）
    const std::vector<int> arr = {1, 3, -1, -3, 5, 3, 6, 7};
    const int k = 3;
    std::deque<int> win;
    for (int i = 0; i < static_cast<int>(arr.size()); ++i) {
        while (!win.empty()) {
            int back_idx = win.back();
            if (arr[static_cast<std::size_t>(back_idx)] <= arr[static_cast<std::size_t>(i)]) {
                win.pop_back();
            } else {
                break;
            }
        }
        win.push_back(i);

        while (!win.empty()) {
            if (win.front() <= i - k) {
                win.pop_front();
            } else {
                break;
            }
        }

        if (i + 1 >= k) {
            int idx = win.front();
            std::cout << "max in window ending " << i << " = "
                      << arr[static_cast<std::size_t>(idx)] << "\n";
        }
    }

    return 0;
}
```

## 双向链表 `std::list`


> `std::list` 是经典的双向链表：节点散落在堆上，用指针串起来，所以没有 `[]`，访问第 n 个元素只能走迭代器。

好处也很实在：在 **已知迭代器的位置 插入或删除是 O(1)** ，而且不会像 `vector` 那样一扩容就全体迭代器作废。

`splice` 还能把另一条链表整段「接」过来，常数时间、不拷贝节点。  
反过来，如果你大部分时间只是在 **顺序扫一遍**，`list` 往往跑不过 `vector`，缓存不友好是链表的老毛病。  
需要随机下标请用 `vector` 或 `deque`；
只要单向、想再省一点指针，可以看 `forward_list`。


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

## 单向链表 `std::forward_list`

> `forward_list` 是「瘦身版 `list`」：只有向前的指针，省一点内存，接口却更绕

- 插入删除多在 **`insert_after` / `erase_after`** 上完成，还得习惯 **`before_begin()`** 这个「首元素之前的哨兵」。
- 为了省空间，甚至**不要求**实现维护 `size()`，想知道长度就自己数或额外维护计数。
- 没有 `push_back`：尾插要一路走到尾，成本摆在那里。

若你老要在某个元素**前面**插、又不想维护前驱指针，不如换 `list` 省心


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

## 字典 `std::map`


> `std::map` 是最常用的「键 → 值」容器：像一本按关键字排序的字典，`每个键只出现一次`。

业务里做配置表、计数器、索引，第一反应常常就是它。  
底层一般是平衡树，查找、插入、删除都是 O(log n) 量级，**键排序遍历**，支持键区间裁剪。

- 若你 **不在乎顺序、只要大表平均更快**，可以看 `unordered-map`;
- 若一个键底下要挂 **多条** 记录，就该换 `multimap`。

元素类型是 **`std::pair<const K, V>`** ：键带 `const`，防止改键把整棵树顺序弄崩。

!> **特别需要注意：** `map` 的 `m[k]` 在键不存在时会 *顺手插入* 一个默认构造的值。


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

## 复字典 `std::multimap`

> `multimap` 和 `map` 听起来像双胞胎，差别却只有一句：**同一个键可以出现很多次**。

- 电话簿里一个人两条号码、日志里同一用户连刷多条记录，这种一对多用 `map` 得自己再套 `vector`；
- 用 `multimap`，标准库会把相同键的元素排在一起，`equal_range` 一拉就是一整段。
- 它没有 `operator[]`，一个键底下挂着好几条值时，`m[k]` 根本说不清该读写哪一条，标准干脆禁掉了。

> 元素仍是 `pair<const K, V>`，排序只看键
- 删的时候要留神：`erase(k)` 会**一锅端**删掉所有这个键；
- 若只想动其中一条，先用 `find` 拿迭代器再 `erase(it)`。

若键必须唯一，请回到 `map.md`。更高标准里还有哈希版的多重映射；

在 C++11 语境下，你至少先记牢：它**有序**，适合既要一对多又要按键扫的场景。

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

## 无序字典 `std::unordered_map`

> `unordered_map` 是哈希表版字典：平均意义上查找、插入、删除都很快，但 **遍历出来不按键排序**。

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

## 集合 `std::set`

> 可以把 `std::set` 想成「只关心键、不关心值」的 `map`：里面是一堆**互不相同**的元素，从小到大排好队。

- 适合去重、判断出现过没有、又希望遍历结果**有序**的场景
- 允许重复键用 `multiset`；
- 不要顺序、只要哈希的平均速度，用 `unordered-set`。

插入重复键时，`insert` 会返回 `pair<iterator,bool>` 告诉你「有没有真的插进去」——没插进去说明原来就有。

> 比较器是**严格弱序**，和 `map` 一样。

这里的「比较器」指 `set<K, Compare>` 里的 `Compare`：一个“决定谁排在谁前面”的规则，通常长这样：

```cpp
struct Compare {
    bool operator()(const K& a, const K& b) const; // a 是否应该排在 b 前面？
};
```

`set` 的默认比较器是 `std::less<K>`（一般等价于用 `<` 做升序）。`set` 的“有序”和“去重”都只看比较器，并不依赖 `==`。

所谓**严格弱序**（strict weak ordering），你可以把它理解成“像正常的排序规则一样靠谱”，至少要满足这些直觉性质：

- **不能自己比自己小**：`comp(x, x)` 必须为 `false`
- **传递性**：若 `comp(a,b)` 且 `comp(b,c)`，则必须 `comp(a,c)`
- **等价也要一致**：若 `a` 与 `b` 等价、`b` 与 `c` 等价，则 `a` 与 `c` 也必须等价


| 成员 | 功能与用法 |
|------|------------|
| `set<K> s` | 空集。 |
| `set<K,Compare> s` | 自定义键比较。 |
| `s.insert(x)` / `s.emplace(args...)` | 插入；已存在则不变（`insert` 返回 `pair<iterator,bool>`）。 |
| `s.erase(it)` / `s.erase(first,last)` | 按迭代器删。 |
| `s.erase(k)` | 按键删，返回删除个数（0 或 1）。 |
| `s.find(k)` | 查找，无则 `end()`。 |
| `s.count(k)` | 0 或 1。 |
| `s.lower_bound` / `s.upper_bound` / `s.equal_range` | 有序区间定位，语义同 `map` 的键侧。 |
| `s.begin()` / `s.end()` | 升序遍历。 |
| `s.empty()` / `s.size()` / `s.clear()` / `s.swap` | 与其它容器类似。 |


```cpp
#include <iostream>
#include <set>
#include <string>

int main() {
    std::set<std::string> names;

    names.insert("bob");
    names.insert("alice");
    names.insert("carol");
    names.insert("alice");

    if (names.count("alice") > 0) {
        std::cout << "alice in set\n";
    }

    std::cout << "sorted names:";
    for (auto it = names.begin(); it != names.end(); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << "\n";

    if (names.erase("bob") > 0) {
        std::cout << "removed bob\n";
    }
    std::cout << "after erase bob:";
    for (auto it = names.begin(); it != names.end(); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << "\n";

    std::set<int> nums;
    nums.insert(5);
    nums.insert(1);
    nums.insert(3);
    std::set<int>::iterator lb = nums.lower_bound(2);
    if (lb != nums.end()) {
        std::cout << "lower_bound(3) points to " << *lb << "\n";
    }

    return 0;
}
```

## 复集合 `std::multiset`


> `multiset` 就是允许重复的 `set`：只有键、没有映射值，但同一个值可以出现很多遍。

做「多重集」、中位数流、带重复的排名时，它比 `set` 顺手，`count` 直接告诉你堆了几层。


| 成员 | 功能与用法 |
|------|------------|
| `multiset<K> ms` | 空多重集。 |
| `ms.insert(x)` / `ms.emplace(...)` | 插入，允许与已有键相等。 |
| `ms.erase(it)` | 删迭代器指向的**一条**。 |
| `ms.erase(k)` | 删光所有等于 `k` 的元素，返回删除个数。 |
| `ms.count(k)` | 等于 `k` 的元素个数。 |
| `ms.equal_range(k)` | 所有等于 `k` 的迭代器区间。 |
| `ms.find(k)` | 指向其中一条等于 `k` 的元素。 |
| `ms.lower_bound` / `ms.upper_bound` | 有序定位。 |
| `ms.begin` / `ms.end` / `ms.size` / `ms.empty` / `ms.clear` / `ms.swap` | 常规。 |


```cpp
#include <iostream>
#include <set>

int main() {
    std::multiset<int> bag;

    bag.insert(3);
    bag.insert(1);
    bag.insert(4);
    bag.insert(1);
    bag.insert(5);
    bag.insert(1);

    std::cout << "count(1)=" << bag.count(1) << "\n";

    std::cout << "sorted:";
    for (auto it = bag.begin(); it != bag.end(); ++it) {
        std::cout << ' ' << *it;
    }
    std::cout << "\n";

    auto one = bag.find(1);
    if (one != bag.end()) {
        bag.erase(one);
    }

    std::cout << "after erasing one instance of 1, count(1)=" << bag.count(1) << "\n";

    if (bag.erase(5) > 0) {
        std::cout << "erased all 5\n";
    }

    return 0;
}
```

## 无序集合 `std::unordered_set`

> `unordered_set` 只存键、不存值，底层哈希，**无序**、平均常数时间查找——可以理解为「去重集合的哈希版」。

经典用法：`insert` 完看一眼返回的 `pair` 里 `second` 是不是 `true`，就知道是不是第一次见到这个元素。

| 成员 | 功能与用法 |
|------|------------|
| `unordered_set<K> s` | 空集。 |
| `s.insert(x)` / `s.emplace(args...)` | 插入；返回 `pair<iterator,bool>` 可知是否新插入。 |
| `s.erase(it)` / `s.erase(k)` | 删除。 |
| `s.find(k)` / `s.count(k)` | 查找、计数（0 或 1）。 |
| `s.reserve` / `s.rehash` | 控制 rehash，减少扩容次数。 |
| `s.bucket_count` / `load_factor` 等 | 观察桶与负载。 |
| `s.begin` / `s.end` / `empty` / `size` / `clear` / `swap` | 常规。 |


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

## 栈 `std::stack`


> `stack` 不是新容器，而是**适配器**：底下默认塞一个 `deque`，也可换成 `vector`、`list`，对外只露出「压栈、弹栈、看栈顶」

想手滑去改栈中间？门都没有，接口本身就是注释。

括号匹配、DFS 手写栈、撤销栈，全是老熟人。

> 记牢一件事：`pop` 不返回值，习惯上先看 `empty()`，再 `top` 取栈顶，最后 `pop`，别在空栈上硬弹。


| 成员 | 功能与用法 |
|------|------------|
| `stack<T> st` | 默认底层 `deque<T>`。 |
| `stack<T, Container> st` | 指定底层序列容器。 |
| `st.push(x)` / `st.emplace(args...)` | 入栈（C++11 `emplace`）。 |
| `st.pop()` | 出栈顶，无返回值。 |
| `st.top()` | 栈顶引用（空栈未定义）。 |
| `st.empty()` / `st.size()` | 判空、元素个数。 |


```cpp
#include <iostream>
#include <stack>
#include <string>
#include <vector>

static bool brackets_ok(const std::string& s) {
    std::stack<char> st;
    for (std::size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c == '(' || c == '[' || c == '{') {
            st.push(c);
        } else if (c == ')' || c == ']' || c == '}') {
            if (st.empty()) {
                return false;
            }
            char t = st.top();
            st.pop();
            bool ok = false;
            if (c == ')' && t == '(') {
                ok = true;
            }
            if (c == ']' && t == '[') {
                ok = true;
            }
            if (c == '}' && t == '{') {
                ok = true;
            }
            if (!ok) {
                return false;
            }
        } else {
            // 其它字符忽略
        }
    }
    if (!st.empty()) {
        return false;
    }
    return true;
}

int main() {
    std::stack<int, std::vector<int> > st;
    for (int v = 1; v <= 3; ++v) {
        st.push(v * 10);
    }

    std::cout << "stack pop order:";
    while (!st.empty()) {
        int x = st.top();
        st.pop();
        std::cout << ' ' << x;
    }
    std::cout << "\n";

    if (brackets_ok("([]{})")) {
        std::cout << "brackets ok\n";
    }
    if (!brackets_ok("([)]")) {
        std::cout << "brackets bad\n";
    }

    return 0;
}
```

## 队列 `std::queue`

> `queue` 也是适配器：默认用 `deque` 垫底，一头进、另一头出，标准的 **FIFO 排队** 语义。


| 成员 | 功能与用法 |
|------|------------|
| `queue<T> q` | 默认底层 `deque<T>`。 |
| `queue<T, Container> q` | 指定底层容器。 |
| `q.push(x)` / `q.emplace(args...)` | 队尾入队。 |
| `q.pop()` | 队头出队，无返回值。 |
| `q.front()` / `q.back()` | 队头、队尾引用（空队列未定义）。 |
| `q.empty()` / `q.size()` | 判空、长度。 |


```cpp
#include <iostream>
#include <queue>
#include <vector>

int main() {
    std::queue<int> q;

    for (int i = 1; i <= 5; ++i) {
        q.push(i);
    }

    std::cout << "front=" << q.front() << " back=" << q.back() << "\n";

    std::cout << "dequeue order:";
    while (!q.empty()) {
        int x = q.front();
        q.pop();
        std::cout << ' ' << x;
    }
    std::cout << "\n";

    // 简单层序：完全二叉树数组 1..n，孩子 2i, 2i+1
    const std::vector<int> tree = {0, 1, 2, 3, 4, 5, 6, 7};
    std::queue<int> bfs;
    if (tree.size() > 1) {
        bfs.push(1);
    }

    while (!bfs.empty()) {
        int level = static_cast<int>(bfs.size());
        for (int k = 0; k < level; ++k) {
            int idx = bfs.front();
            bfs.pop();
            if (idx >= 0 && static_cast<std::size_t>(idx) < tree.size()) {
                std::cout << tree[static_cast<std::size_t>(idx)] << ' ';
            }
            int left = idx * 2;
            int right = left + 1;
            if (left < static_cast<int>(tree.size())) {
                bfs.push(left);
            }
            if (right < static_cast<int>(tree.size())) {
                bfs.push(right);
            }
        }
        std::cout << "\n";
    }

    return 0;
}
```

## 优先队列 `std::priority_queue`


> `priority_queue` 默认是 **`vector` + 二叉堆**：堆顶 `top()` 总是「最优先」的那个。

默认比较是 `std::less<T>`，数字上就是**大顶堆**——最大的在顶上。  
要写小顶堆，把比较器换成 `std::greater<T>`（记得 `#include <functional>`）。

第三模板参数 **`Compare`** 的语义最容易写反：它决定「谁该在堆顶当老大」，建议写个三五行的小程序打印几次 `top`，确认无误再写进项目。


| 成员 | 功能与用法 |
|------|------------|
| `priority_queue<T> pq` | 默认 `vector<T>` + `less<T>`（大顶堆）。 |
| `priority_queue<T, vector<T>, greater<T> > pq` | 小顶堆（`greater` 在 `<functional>`）。 |
| `pq.push(x)` / `pq.emplace(args...)` | 入堆。 |
| `pq.pop()` | 弹出堆顶，无返回值。 |
| `pq.top()` | 堆顶引用（空堆未定义）。 |
| `pq.empty()` / `pq.size()` | 判空、元素个数。 |

```cpp
#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

struct Task {
    std::string name;
    int priority;
};

struct TaskCompare {
    bool operator()(const Task& a, const Task& b) const {
        if (a.priority < b.priority) {
            return true;
        }
        if (a.priority > b.priority) {
            return false;
        }
        return false;
    }
};

int main() {
    std::priority_queue<int> big_top;
    big_top.push(3);
    big_top.push(1);
    big_top.push(4);
    std::cout << "max-heap pop:";
    while (!big_top.empty()) {
        int v = big_top.top();
        big_top.pop();
        std::cout << ' ' << v;
    }
    std::cout << "\n";

    std::priority_queue<int, std::vector<int>, std::greater<int> > small_top;
    small_top.push(3);
    small_top.push(1);
    small_top.push(4);
    std::cout << "min-heap pop:";
    while (!small_top.empty()) {
        int v = small_top.top();
        small_top.pop();
        std::cout << ' ' << v;
    }
    std::cout << "\n";

    std::priority_queue<Task, std::vector<Task>, TaskCompare> jobs;
    jobs.push(Task{"log", 1});
    jobs.push(Task{"fire", 9});
    jobs.push(Task{"mail", 3});

    std::cout << "jobs by priority:";
    while (!jobs.empty()) {
        Task t = jobs.top();
        jobs.pop();
        std::cout << ' ' << t.name << '(' << t.priority << ')';
    }
    std::cout << "\n";

    return 0;
}
```

## 二元组 `std::pair`


> `std::pair<T1, T2>` 就是一个“装两个值”的小结构体：  
它有两个公开成员 `first` / `second`，常用来表示**二元组**、**键值对**、或者函数返回“两个结果”。

最常见的出现场景：

- 三元及以上异质打包见 [tuple](#std-tuple)；
- `map` / `unordered_map` 的元素类型就是 `std::pair<const Key, T>`；
- 算法里常把“值 + 下标/来源”打包（比如最小值与位置）；
- 函数想返回两个值时，不想为了这点东西专门写一个结构体；

需要注意的几个点：

- `pair` 的比较是**字典序（lexicographical order）**：先比 `first`，相等再比 `second`。
- `std::make_pair(...)` 会做类型推导；但遇到引用/数组/字符串字面量时，推导结果可能与你想的不一样（例如 `"hi"` 会推成 `const char*`）。


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

## 多元组 `std::tuple`

`std::tuple` 用于固定长度、元素类型可异的有序组合；按下标访问使用 **`std::get<I>`**（`I` 为编译期整型常量）。`std::make_tuple` 与 `std::tie` 的常见用法与 `std::pair` 小节中的 `tie` / `ignore` 说明一致。

| 用法 | 说明 |
|------|------|
| `std::tuple<T...> t(args...)` | 直接构造。 |
| `std::make_tuple(...)` | 按实参推导元素类型。 |
| `std::get<I>(t)` | 按下标取元素。 |
| `std::tie(a,b,...)=t` | 解包到已有变量。 |

```cpp
#include <iostream>
#include <string>
#include <tuple>

std::tuple<int, std::string, double> make_row() {
    return std::make_tuple(1, std::string("alpha"), 2.5);
}

int main() {
    auto row = make_row();
    std::cout << std::get<0>(row) << ' ' << std::get<1>(row) << ' ' << std::get<2>(row) << '\n';

    int id = 0;
    std::string name;
    double score = 0.0;
    std::tie(id, name, score) = row;
    std::tie(std::ignore, name, std::ignore) = row;

    return 0;
}
```

----
# 算法

> 标准库算法的价值在于：你不必每次都重新手写循环。

排序、查找、计数、复制、删除条件匹配元素，这些动作本来就很通用，所以标准库把它们做成了独立算法。

例如排序：

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{5, 1, 4, 3, 2};
    std::sort(nums.begin(), nums.end());

    for (auto n : nums) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    return 0;
}
```

再看一个和 `lambda` 配合的例子：

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{1, 2, 3, 4, 5};
    auto count = std::count_if(nums.begin(), nums.end(), [](int x) {
        return x % 2 == 0;
    });

    std::cout << count << std::endl;
    return 0;
}
```

这就是 `C++11` 之后非常典型的标准库风格：

- 容器存数据
- 算法处理数据
- `lambda` 描述条件或规则

> 顺手建立一个阅读习惯：看到容器时，不要只盯着“里面装了什么”，还要继续看“后面打算配什么算法一起用”。

与随机抽样、文本模式匹配相关的标准库选型，见 [随机与正则](../std/random-regex.md)。

----
# 迭代器

想把标准库真正看顺，迟早要接受一个事实：**很多算法并不是直接操作容器，而是操作迭代器。**

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{3, 1, 2, 4};
    auto it = std::find(nums.begin(), nums.end(), 2);

    if (it != nums.end()) {
        std::cout << *it << std::endl;
    }
    return 0;
}
```

你可以先把迭代器理解成 `容器给算法提供的统一访问入口`。这也是为什么很多标准库算法都长得像：

```cpp
algorithm(begin, end, ...);
```

它们不急着知道容器的具体类型，而是先通过区间来工作。

## 操作区间

这里最值得先记住的是：`[begin, end)` 表示 **左闭右开区间**。

- `begin` 指向第一个元素
- `end` 不是最后一个元素
- **`end` 指向的是“最后一个元素的下一个位置”**

这也是为什么很多算法判断“没找到”时，返回的都是 `end()`：

```cpp
auto it = std::find(nums.begin(), nums.end(), 100);
if (it == nums.end()) {
    std::cout << "not found" << std::endl;
}
```

一旦接受了这个规则，很多接口都会突然变得统一起来。
比如“遍历整个容器”“查找一段区间”“复制一段内容”，本质上都只是在处理一个 `[begin, end)`。

## 返回迭代器

算法返回迭代器，而不是直接返回下标或元素本身，是因为它想尽量通用。

- 有的容器本来就没有“下标”这个概念，例如 `list`
- 返回元素值会丢失“位置”信息
- 返回迭代器后，调用者还能继续在那个位置上操作

例如：

```cpp
auto it = std::find(nums.begin(), nums.end(), 2);
if (it != nums.end()) {
    *it = 20;
}
```

> 这类写法背后的意思是：算法只负责把“位置”交给你，至于后面要读、改、删，决定权留给调用方。

## 迭代器能力

迭代器虽然提供了统一接口，但不同容器给出的“能力”并不完全一样。

- `vector` 的迭代器支持随机访问
- `list` 的迭代器通常只能双向移动
- 有些算法要求更强的迭代器能力

这会直接影响算法能不能用。例如下面这种写法对 `vector` 很自然：

```cpp
std::sort(nums.begin(), nums.end());
```

但如果换成 `list`，就不能直接这样写，因为 `std::sort` 需要随机访问迭代器，而 `list` 不提供这种能力。

所以读标准库代码时，除了问“这是什么容器”，还可以继续多问一句：**它给算法提供的是哪一类迭代器能力？**

----
# 容器怎么选

把容器、算法、迭代器放在一起看之后，选容器时的判断会更稳定：

- 大多数情况下先考虑 `vector`
- 需要按 key 管理数据时考虑 `map` / `unordered_map`
- 需要元素唯一时考虑 `set`
- 需要双端高效操作时考虑 `deque`
- 只有在确实适合时再考虑 `list`

> 不要在没有证据时因为“理论复杂度”过早选择冷门容器。

更实际的问法通常是：

- 我主要是顺序遍历，还是按 key 查找
- 我后面会不会大量排序、查找、过滤
- 我需不需要随机访问
- 我是否依赖元素唯一或有序

----
# 失效意识

容器章节还有一个很容易在项目里踩坑的点：**你保存下来的引用、指针或迭代器，不一定会一直有效。**

例如对 `vector` 来说，如果后续插入元素导致扩容，之前拿到的旧地址、旧引用、旧迭代器就可能失效。

所以一旦你打算长期保存这些“指向容器内部元素的东西”，就应该顺手问自己：

- 后面还会继续插入吗
- 容器会不会重新分配内存
- 这份引用、指针或迭代器现在是不是已经不安全了

这个意识之所以重要，正是因为容器、算法、迭代器经常混在一起使用。一旦位置失效，后续算法代码也可能一起出问题。

----
# 小结

这一章真正要建立的，不是“我记住了多少容器名字”，而是“我能不能把容器、算法、迭代器当成一套配合关系来理解”。

- 容器负责存数据
- 算法负责处理数据
- 迭代器负责描述区间和位置
- 选容器时优先考虑后续算法怎么配合
- 保存引用、指针、迭代器时顺手考虑失效问题

只要把这条主线抓住，后面再学习更多标准库内容时，就不会只看到零碎 API，而会看到它们之间的组织方式。
