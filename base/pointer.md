# 指针

指针是 `C++` 最核心、也最危险的基础能力之一。你以前学的指针知识并没有过时，但接手 `C++11` 项目时，最大的变化是：裸指针不再默认等于“所有权”。这会彻底影响你理解代码的方式。

## 指针的基本概念

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

## 空指针

旧代码里常写：

```cpp
int* ptr = NULL;
```

`C++11` 推荐统一使用：

```cpp
int* ptr = nullptr;
```

原因是 `nullptr` 具有明确的空指针语义，不会像 `NULL` 那样在重载解析中被误当成整数。

## 指针与数组

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

## `const` 与指针

```cpp
const int* p1 = nullptr;
int* const p2 = nullptr;
const int* const p3 = nullptr;
```

这是阅读项目代码必须具备的基础能力：

- `const int*`：不能通过指针改数据
- `int* const`：指针本身不能改指向
- `const int* const`：两者都不行

## 动态内存与 `new/delete`

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

## 智能指针

`C++11` 最重要的指针升级之一，就是标准库智能指针。

### `std::unique_ptr`

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

### `std::shared_ptr`

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

## 裸指针在 C++11 中的角色

现代 `C++` 并不是“彻底不用裸指针”，而是重新定义其语义：

- 裸指针常用于非拥有关系
- 表示“可能为空的观察者”
- 与底层接口或 C API 交互

而“拥有资源并负责释放”的语义，应优先交给智能指针或容器。

## 一个综合示例

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

## 推荐实践

- 空指针统一使用 `nullptr`。
- 能不用裸 `new/delete` 就不用。
- 独占所有权优先使用 `std::unique_ptr`。
- 共享所有权确有必要时再用 `std::shared_ptr`。
- 裸指针更多用于“观察”而不是“拥有”。

## 小结

学习指针时最需要更新的不是取地址和解引用本身，而是“所有权模型”。`C++11` 之后，代码是否安全，很大程度上取决于你有没有把资源所有权表达清楚，而这正是智能指针出现的意义。
