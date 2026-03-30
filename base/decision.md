# 条件

条件控制在语法层面非常基础，但在现代 `C++` 里，条件表达式的书写风格已经明显更强调类型安全、意图清晰和局部作用域控制。

# if

最基础的条件分支：

```cpp
#include <iostream>

int main() {
    int score = 85;

    if (score >= 60) {
        std::cout << "pass" << std::endl;
    }

    return 0;
}
```

`if` 条件要求能转换成布尔值，但现代代码中应尽量直接使用 `bool` 表达式，而不是依赖各种隐式真假规则。

# if...else

```cpp
#include <iostream>

int main() {
    int score = 58;

    if (score >= 60) {
        std::cout << "pass" << std::endl;
    } else {
        std::cout << "fail" << std::endl;
    }

    return 0;
}
```

# else if

```cpp
#include <iostream>

int main() {
    int score = 88;

    if (score >= 90) {
        std::cout << "A" << std::endl;
    } else if (score >= 80) {
        std::cout << "B" << std::endl;
    } else if (score >= 60) {
        std::cout << "C" << std::endl;
    } else {
        std::cout << "D" << std::endl;
    }

    return 0;
}
```

# switch

当条件是离散枚举值时，`switch` 往往更清晰：

```cpp
#include <iostream>

int main() {
    int day = 2;

    switch (day) {
        case 1:
            std::cout << "Mon" << std::endl;
            break;
        case 2:
            std::cout << "Tue" << std::endl;
            break;
        default:
            std::cout << "Other" << std::endl;
            break;
    }

    return 0;
}
```

# enum class 与 switch

`C++11` 更推荐使用强类型枚举，再配合 `switch`：

```cpp
#include <iostream>

enum class Command {
    Start,
    Stop,
    Pause
};

int main() {
    Command cmd = Command::Start;

    switch (cmd) {
        case Command::Start:
            std::cout << "start" << std::endl;
            break;
        case Command::Stop:
            std::cout << "stop" << std::endl;
            break;
        case Command::Pause:
            std::cout << "pause" << std::endl;
            break;
    }

    return 0;
}
```

# 常见陷阱

## 赋值与比较

```cpp
int x = 10;

if (x = 0) {
    // 错误：这是赋值，不是比较
}
```

编译器警告通常能发现这种问题，所以建议始终开启 `-Wall -Wextra`。

## 指针真假判断

```cpp
int* p = nullptr;
// 可以工作，但更推荐明确比较语义
if (p) {
}

// 更明确的写法：
if (p != nullptr) {
}
```

## 复杂逻辑

当条件非常复杂时，先拆成有名字的布尔变量通常更清楚：

```cpp
bool isLoggedIn = true;
bool hasPermission = true;
bool isExpired = false;

bool canAccess = isLoggedIn && hasPermission && !isExpired;
```

# 综合示例

```cpp
#include <iostream>
#include <string>

int main() {
    int age = 20;
    bool hasTicket = true;

    if (age >= 18 && hasTicket) {
        std::cout << "allow entry" << std::endl;
    } else {
        std::cout << "deny entry" << std::endl;
    }

    std::string result = (age >= 18) ? "adult" : "minor";
    std::cout << result << std::endl;
    return 0;
}
```

写条件时，可以多问一句“读代码的人会不会立刻知道我在判断什么”。
> 当条件开始同时承担空值检查、状态判断和业务规则时，优先拆出具名布尔变量，通常比追求一行写完更稳妥。

`if/else` 和 `switch` 也不是谁更高级，而是谁更适合当前问题：
- 连续范围判断更适合 `if/else`
- 离散状态分发更适合 `switch`。

# 推荐实践

- 条件表达式优先写成清晰的 `bool` 逻辑。
- `switch` 配合 `enum class` 使用时，可读性通常很好。
- 复杂条件先拆成具名布尔变量。
- 打开编译警告，避免赋值和比较混淆。
- 判断空指针时统一使用 `nullptr`。

# 小结

- 条件控制的难点从来不只是语法，而是把分支意图写清楚。
- 条件越清楚，后续重构、调试和排查问题就越轻松；
- 这也是现代 `C++` 在条件表达式上最强调的习惯。
