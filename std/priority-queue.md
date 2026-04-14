# priority_queue

## 1 概述

`priority_queue` 默认是 **`vector` + 二叉堆**：堆顶 `top()` 总是「最优先」的那个。默认比较是 `std::less<T>`，数字上就是**大顶堆**——最大的在顶上。要写小顶堆，把比较器换成 `std::greater<T>`（记得 `#include <functional>`）。第三模板参数 **`Compare`** 的语义最容易写反：它决定「谁该在堆顶当老大」，建议写个三五行的小程序打印几次 `top`，确认无误再写进项目。

和 `set` 比：取极值 `top` 是 O(1)，但想有序遍历所有元素就很别扭——适合调度、多路归并、Dijkstra 这种「只要最值」的场景。

头文件 `<queue>`（与 `queue` 共用）。模板 `priority_queue<T, Container, Compare>` 里，`Container` 一般用 `vector`，要满足堆算法对序列的要求。

----

## 2 接口

| 成员 | 功能与用法 |
|------|------------|
| `priority_queue<T> pq` | 默认 `vector<T>` + `less<T>`（大顶堆）。 |
| `priority_queue<T, vector<T>, greater<T> > pq` | 小顶堆（`greater` 在 `<functional>`）。 |
| `pq.push(x)` / `pq.emplace(args...)` | 入堆。 |
| `pq.pop()` | 弹出堆顶，无返回值。 |
| `pq.top()` | 堆顶引用（空堆未定义）。 |
| `pq.empty()` / `pq.size()` | 判空、元素个数。 |

----

## 3 示例

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
