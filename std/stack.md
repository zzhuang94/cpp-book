# stack

## 概述

> `stack` 不是新容器，而是**适配器**：底下默认塞一个 `deque`，也可换成 `vector`、`list`，对外只露出「压栈、弹栈、看栈顶」

想手滑去改栈中间？门都没有，接口本身就是注释。

括号匹配、DFS 手写栈、撤销栈，全是老熟人。

> 记牢一件事：`pop` 不返回值，习惯上先看 `empty()`，再 `top` 取栈顶，最后 `pop`，别在空栈上硬弹。

----

## 接口

| 成员 | 功能与用法 |
|------|------------|
| `stack<T> st` | 默认底层 `deque<T>`。 |
| `stack<T, Container> st` | 指定底层序列容器。 |
| `st.push(x)` / `st.emplace(args...)` | 入栈（C++11 `emplace`）。 |
| `st.pop()` | 出栈顶，无返回值。 |
| `st.top()` | 栈顶引用（空栈未定义）。 |
| `st.empty()` / `st.size()` | 判空、元素个数。 |

----

## 示例

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
