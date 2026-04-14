# deque

## 概述

> `deque`（读作 deck）常被叫「双端队列」：**两头 `push`、`pop` 都顺手** ，中间还能用 **`[]`** 当数组使。

- 和 `vector` 不同，它 **不保证** 一整块连续内存，所以别把 `&dq[0]` 当成能一把 `memcpy` 到底的指针；
- 和 `list` 比，它又保留了随机访问的甜头。

滑动窗口、单调队列里，它几乎是标配配角。中间插入删除要花多少代价，跟具体标准库实现有关，一般夹在 `vector` 和 `list` 之间

如果 **只在尾部** 折腾，`vector` 往往更简单；若 **两头都动** 还要下标，再把 `deque` 请上台。

----

## 接口

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

----

## 示例

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
