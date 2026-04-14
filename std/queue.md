# queue

## 1 概述

`queue` 也是适配器：默认用 `deque` 垫底，一头进、另一头出，标准的 **FIFO 排队** 语义。写 BFS、任务队列、流水缓冲时很贴切。标准接口里**没有**「从头到尾扫一遍队列」——设计上就不鼓励你插队乱看；真要遍历，换容器或自己包一层。队列为空时别 `pop` 也别 `front`，先看 `empty()`，和栈同一个脾气。

头文件 `<queue>`。底层可换成 `list`，例如 `std::queue<T, std::list<T> >`，由 `list` 保证两端 O(1) 插入删除。

----

## 2 接口

| 成员 | 功能与用法 |
|------|------------|
| `queue<T> q` | 默认底层 `deque<T>`。 |
| `queue<T, Container> q` | 指定底层容器。 |
| `q.push(x)` / `q.emplace(args...)` | 队尾入队。 |
| `q.pop()` | 队头出队，无返回值。 |
| `q.front()` / `q.back()` | 队头、队尾引用（空队列未定义）。 |
| `q.empty()` / `q.size()` | 判空、长度。 |

----

## 3 示例

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
