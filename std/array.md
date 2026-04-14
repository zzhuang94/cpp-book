# array

## 概述

`std::array<T, N>` 是 **C++11** 引入的定长顺序容器，头文件 `<array>`。

它像是给裸数组套了一层标准库外壳：长度 **`N` 写死在类型里**，  
既不能 `push_back`，也不会偷偷 realloc，适合 RGB、固定长度缓冲、小矩阵行这种「编译期就知道要几个」的场景。

把它嵌在结构体里当成员，往往比塞一个 `vector` 更直白，少一次堆分配的心理负担。  
代价是：`N` 不同的两个 `array` 根本不是同一个类型，没法互相赋值；

`array<T,0>` 在标准里是合法的怪胎，`front`/`back` 就别乱碰了。把元素交给只认指针的 C 函数时，用 `data()` 取首地址；若是空数组，别解引用。初始化时若类型推导犯迷糊，可以用双层花括号 `{{...}}` 和 `initializer_list` 拉开距离。

----

## 接口

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

----

## 示例

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
