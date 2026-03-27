# 条件

条件控制在语法层面非常基础，但在现代 `C++` 里，条件表达式的书写风格已经明显更强调类型安全、意图清晰和局部作用域控制。

# `if`

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

# `if...else`

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

# `else if`

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

# `switch`

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

# `enum class` 与 `switch`

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

if (p) {
    // 可以工作，但更推荐明确比较语义
}
```

更明确的写法：

```cpp
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

# 推荐实践

- 条件表达式优先写成清晰的 `bool` 逻辑。
- `switch` 配合 `enum class` 使用时，可读性通常很好。
- 复杂条件先拆成具名布尔变量。
- 打开编译警告，避免赋值和比较混淆。
- 判断空指针时统一使用 `nullptr`。

# 小结

条件控制的语法很基础，但优秀的 `C++11` 代码会把“意图表达”放在第一位。条件越清楚，后续重构、调试和阅读就越轻松。

## 条件语句在工程代码里的真正价值

在真实项目里，条件语句很少只是“语法题”。它往往承担着：

- 表达业务分支
- 防守非法输入
- 控制资源释放时机
- 保护对象状态不被错误修改

因此，条件写得清不清楚，会直接影响后续维护。

## 优先写“能读懂的条件”

例如这段代码：

```cpp
if (user != nullptr && user->isActive() && !user->isExpired()) {
}
```

虽然不算复杂，但如果条件再继续增长，最好提炼成具名变量：

```cpp
bool hasUser = (user != nullptr);
bool isActiveUser = hasUser && user->isActive();
bool canAccess = isActiveUser && !user->isExpired();

if (canAccess) {
}
```

这样做不是啰嗦，而是让业务意图更明确。

## `switch` 最适合离散状态，而不是硬凑所有分支

当你的判断对象是枚举状态时，`switch` 很合适；但如果条件里混杂了范围判断、对象状态、空值判断，`if/else` 往往更自然。

现代 `C++` 里一个很好的组合是：

- 用 `enum class` 表达状态
- 用 `switch` 做离散分发

这样既减少魔法数字，也让状态边界更清楚。

## 空指针判断时尽量明确

虽然下面这样能工作：

```cpp
if (ptr) {
}
```

但对初学者和阅读者来说，下面往往更直接：

```cpp
if (ptr != nullptr) {
}
```

尤其在条件变复杂时，显式比较可读性更好。

## 常见误区

### 误区 1：条件越短越好

不对。读得懂比机械地短更重要。

### 误区 2：只要能用三目运算符就别写 `if`

不对。复杂逻辑强行压成一行通常会降低可读性。

### 误区 3：`switch` 一定比 `if` 更高级

不对。关键要看问题是不是离散枚举分支。

## 补充建议

- 条件复杂时优先拆成具名布尔变量。
- 空指针判断时尽量写出明确语义。
- 离散状态优先用 `enum class + switch`。
- 不要把业务逻辑、边界检查和错误恢复全堆在一个超长条件里。
