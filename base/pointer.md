# 指针

指针是 `C++` 最核心、也最危险的基础能力之一。你以前学的指针知识并没有过时，但接手 `C++11` 项目时，最大的变化是：裸指针不再默认等于“所有权”。这会彻底影响你理解代码的方式。

# 基本概念

指针保存的是地址。

```cpp
#include <iostream>

int main() {
    int value = 10;
    int* ptr = &value;

    std::cout << *ptr << std::endl;
    return 0;
}
```

这里：

- `&value` 取地址
- `int* ptr` 声明指针
- `*ptr` 解引用

# 空指针

旧代码里常写：

```cpp
int* ptr = NULL;
```

`C++11` 推荐统一使用：

```cpp
int* ptr = nullptr;
```

原因是 `nullptr` 具有明确的空指针语义，不会像 `NULL` 那样在重载解析中被误当成整数。

# 指针与数组

```cpp
#include <iostream>

int main() {
    int nums[3] = {1, 2, 3};
    int* p = nums;

    std::cout << *p << std::endl;
    std::cout << *(p + 1) << std::endl;
    return 0;
}
```

数组名在很多表达式里会退化成指向首元素的指针，这也是很多旧代码容易出错的来源之一。

# `const` 与指针

```cpp
const int* p1 = nullptr;
int* const p2 = nullptr;
const int* const p3 = nullptr;
```

这是阅读项目代码必须具备的基础能力：

- `const int*`：不能通过指针改数据
- `int* const`：指针本身不能改指向
- `const int* const`：两者都不行

# 动态内存

传统写法：

```cpp
int* p = new int(42);
delete p;
```

数组形式：

```cpp
int* arr = new int[10];
delete[] arr;
```

这种写法在语法上没有问题，但现代 `C++` 最大的经验教训就是：

> 手动 `new/delete` 很容易导致泄漏、悬空指针、重复释放和异常路径资源丢失。

# 智能指针

`C++11` 最重要的指针升级之一，就是标准库智能指针。

## `std::unique_ptr`

表示独占所有权：

```cpp
#include <iostream>
#include <memory>

int main() {
    std::unique_ptr<int> p(new int(42));
    std::cout << *p << std::endl;
    return 0;
}
```

它不能被随意复制，只能移动。

## `std::shared_ptr`

表示共享所有权：

```cpp
#include <iostream>
#include <memory>

int main() {
    std::shared_ptr<int> p1(new int(100));
    std::shared_ptr<int> p2 = p1;

    std::cout << p1.use_count() << std::endl;
    return 0;
}
```

多个对象共同拥有同一资源时可以使用它，但要警惕循环引用。

# 裸指针角色

现代 `C++` 并不是“彻底不用裸指针”，而是重新定义其语义：

- 裸指针常用于非拥有关系
- 表示“可能为空的观察者”
- 与底层接口或 C API 交互

而“拥有资源并负责释放”的语义，应优先交给智能指针或容器。

# 综合示例

```cpp
#include <iostream>
#include <memory>

void print(const int* p) {
    if (p != nullptr) {
        std::cout << *p << std::endl;
    }
}

int main() {
    int value = 7;
    print(&value);

    std::unique_ptr<int> ptr(new int(42));
    std::cout << *ptr << std::endl;
    return 0;
}
```

# 推荐实践

- 空指针统一使用 `nullptr`。
- 能不用裸 `new/delete` 就不用。
- 独占所有权优先使用 `std::unique_ptr`。
- 共享所有权确有必要时再用 `std::shared_ptr`。
- 裸指针更多用于“观察”而不是“拥有”。

# 小结

学习指针时最需要更新的不是取地址和解引用本身，而是“所有权模型”。`C++11` 之后，代码是否安全，很大程度上取决于你有没有把资源所有权表达清楚，而这正是智能指针出现的意义。

## 指针为什么是其它语言开发者进入 C++ 的最大门槛之一

如果你主要写 `Go`、`PHP`、`JS`、`TS`，你可能不陌生“引用某个对象”的感觉，但 `C++` 指针的难点在于：

- 它既能表示地址
- 也常被历史代码用来表示所有权
- 还可能用来表示可空借用
- 甚至可能只是某个连续内存区间的起点

所以“看到指针”之后，最关键的问题不是“怎么解引用”，而是：

> 这个指针到底代表什么语义？

## 裸指针和所有权必须拆开理解

现代 `C++` 的一个核心观念就是：

- 裸指针不应默认等于拥有资源
- 裸指针更适合作为观察者、可空借用或底层接口桥梁

这和很多旧代码差异非常大，也是阅读现代项目时必须尽快建立的直觉。

## `std::unique_ptr` 应该成为默认拥有语义候选

当你明确知道“某个对象只应该有一个主人”时，`std::unique_ptr` 往往是优先选项。它的价值不只是自动释放，还包括：

- 让拥有关系一眼可见
- 防止随意复制
- 能和移动语义自然配合

也就是说，它是在把“谁负责释放资源”写进类型里。

## `std::shared_ptr` 不该当成“更方便的默认值”

很多初学者会觉得共享所有权听起来更灵活，于是倾向于默认用 `shared_ptr`。但更稳妥的顺序通常是：

1. 先问是否真的需要共享所有权
2. 如果不需要，优先 `unique_ptr`
3. 只有确实有共享生命周期需求时，再考虑 `shared_ptr`

因为共享所有权虽然方便，也意味着：

- 生命周期更难推理
- 计数有额外成本
- 容易引入循环引用

## `weak_ptr` 要先知道它解决什么问题

当两个 `shared_ptr` 互相持有时，引用计数可能永远归零不了。这时 `weak_ptr` 的意义就出现了：

- 它表示“我想观察这个共享对象”
- 但不参与拥有关系计数

对初学者来说，先知道它是“打破循环引用的重要工具”就已经很有用。

## 与 GC 语言的心智差异

如果你来自带垃圾回收的语言，可以这样理解：

- GC 语言里你更多只关心“还有没有引用”
- `C++` 里你更需要关心“谁拥有、谁借用、谁负责销毁”

这不是单纯的内存释放问题，而是整个接口设计思路都不同。

## 一个更接近工程语境的例子

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <utility>

class Logger {
public:
    void write(const std::string& text) const {
        std::cout << text << std::endl;
    }
};

class UserService {
public:
    explicit UserService(std::unique_ptr<Logger> logger)
        : logger_(std::move(logger)) {
    }

    void createUser(const std::string& name) const {
        logger_->write(name);
    }

private:
    std::unique_ptr<Logger> logger_;
};
```

这里读代码时，你能一眼看出：

- `UserService` 拥有 `Logger`
- 拥有关系通过 `unique_ptr` 表达
- 生命周期会随着 `UserService` 一起结束

这种“类型就是文档”的感觉，是现代 `C++` 很大的价值。

## 常见误区

### 误区 1：学指针就是学地址和解引用

不够。更关键的是所有权和生命周期。

### 误区 2：智能指针只是自动 delete

不对。它们更重要的作用是表达语义边界。

### 误区 3：`shared_ptr` 比 `unique_ptr` 更方便，所以该优先用

不建议。共享所有权应该是经过确认后的选择。

### 误区 4：用了智能指针就不会再有生命周期问题

不对。循环引用、借用悬空、错误保存裸指针仍然可能出问题。

## 补充建议

- 一看到指针就先问“这个指针是否拥有资源”。
- 独占所有权优先 `std::unique_ptr`。
- 共享所有权有明确需求时再用 `std::shared_ptr`。
- 观察共享对象但不拥有时考虑 `std::weak_ptr`。
- 与 C API 或非拥有关系交互时，裸指针仍然合理，但语义要想清楚。
