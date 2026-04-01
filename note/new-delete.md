# new / delete

在 C++11 里，`new` 和 `delete` 仍然是最基础的动态内存管理工具。  
它们主要用在**运行时才确定对象数量或对象生命周期**的场景里。

很多初学者第一次接触它们，通常是在下面这些地方：

- 动态创建单个对象
- 动态创建对象数组
- 写链表、树、图这类动态数据结构
- 阅读旧代码或底层代码

如果只记一句话，可以记成：

> `new` 用来分配内存并构造对象，`delete` 用来析构对象并释放内存。

---

# 背景

在 C 语言里，常见的是 `malloc` 和 `free`；  
在 C++ 里，更常见的是 `new` 和 `delete`。

两者最重要的区别不是“名字不同”，而是：

- `new` 不只是申请一块内存，还会调用对象的构造函数
- `delete` 不只是释放内存，还会调用对象的析构函数

这也是为什么，在 C++ 对象语义里，`new/delete` 比 `malloc/free` 更自然。

到了 C++11，这个话题又多了一层背景：

- 语言已经支持 `nullptr`
- 标准库已经提供 `std::unique_ptr` 和 `std::shared_ptr`
- RAII 的写法比早期 C++ 更成熟

> 所以在 C++11 中，`new/delete` 依然重要，但它们更像是**底层机制**；    
> 日常业务代码里，很多时候更推荐把它们交给智能指针管理。

---

# 常见使用方式

## 创建单个对象

```cpp
#include <iostream>
int main() {
    int* p = new int(10);
    std::cout << *p << std::endl;
    delete p;
}
```

这里的 `new int(10)` 做了两件事：
- 分配一块足够存放 `int` 的内存
- 用值 `10` 初始化这块内存中的对象

对应地，`delete p` 会释放这块动态内存。  
如果是自定义类型，还会自动调用构造和析构函数：

```cpp
#include <iostream>

class Test {
public:
    Test() { std::cout << "construct" << std::endl; }
    ~Test() { std::cout << "destruct" << std::endl; }
};

int main() {
    Test* p = new Test;
    delete p;
}
```

## 创建对象数组

```cpp
#include <iostream>

int main() {
    int* arr = new int[5]{1, 2, 3, 4, 5};
    for (int i = 0; i < 5; ++i) {
        std::cout << arr[i] << std::endl;
    }
    delete[] arr;
}
```

这里要特别注意：  **`new[]` 必须配 `delete[]`，不能写成普通的 `delete`。**

> 如果数组元素是类类型，`delete[]` 会负责把每个元素都析构掉。

## 运行时决定大小

动态内存最常见的理由之一，就是“编译时不知道大小”。

```cpp
#include <iostream>

int main() {
    int n = 0;
    std::cin >> n;
    int* arr = new int[n];
    for (int i = 0; i < n; ++i) {
        arr[i] = i * 10;
    }
    delete[] arr;
}
```

这类写法在学习数据结构时很常见。  
不过在实际 C++11 代码里，如果只是想要“可变长度数组”，通常更推荐直接使用 `std::vector`。

## 配合 nullptr

C++11 引入了 `nullptr`，这让空指针表达更清晰。

```cpp
int* p = nullptr;

p = new int(42);
delete p;
p = nullptr;
```

和旧写法里的 `NULL` 相比，`nullptr` 是专门的空指针字面量，语义更明确，也更不容易引发重载歧义。

---

# new 和 delete 的对应关系

最基本的规则只有两条：
- `new` 对应 `delete`
- `new[]` 对应 `delete[]`

可以先直接记成这张表：

| 分配方式 | 释放方式 |
| --- | --- |
| `new T` | `delete p` |
| `new T(...)` | `delete p` |
| `new T[n]` | `delete[] p` |

下面这种写法是错误的：

```cpp
int* arr = new int[10];
delete arr;    // 错误
```

它的问题不是“语法不通过”，而是**行为未定义**。  
也就是说，程序也许能跑，也许会崩，也许会在以后才出问题。

---

# 注意事项

`new/delete` 最容易出问题的地方，不在“会不会写”，而在“会不会长期写对”。

## 不要忘记释放

```cpp
int* p = new int(10);
```

如果后面没有对应的 `delete p;`，这块内存就无法再被正确回收，形成内存泄漏。  
短小示例里问题不明显，但程序一旦长期运行，泄漏会不断累积。

## 不要重复释放

```cpp
int* p = new int(10);

delete p;
delete p;    // 错误
```

同一块内存只能释放一次，重复 `delete` 也是未定义行为。  
更稳妥的习惯是：

```cpp
int* p = new int(10);

delete p;
p = nullptr;
```

> 这样后续代码即使又写了 `delete p;`，对空指针执行 `delete` 也是安全的。

## 数组一定要用 delete[]

这是最常见的配对错误之一：

```cpp
int* arr = new int[5];
delete arr;    // 错误
```

即使元素类型是 `int`，也不应该因为“看起来没出事”就继续这样写。  
如果元素类型换成带析构函数的类，这类错误通常更容易暴露。

## 异常安全要小心

手写 `new/delete` 时，一个常见风险是：  
`new` 之后，如果中间代码抛出异常，后面的 `delete` 可能根本来不及执行。

```cpp
void func() {
    int* p = new int(10);

    // 中间如果抛异常，下面这句可能执行不到
    delete p;
}
```

这也是为什么在 C++11 里，很多场景更推荐用 RAII 和智能指针，而不是手动管理释放时机。

---

# C++11 中更推荐的写法

虽然 `new/delete` 必须会，但在 C++11 中，很多时候更推荐把“释放责任”交给对象本身。

## 优先考虑栈对象

如果对象不需要动态生命周期，直接写局部变量通常最简单：

```cpp
int main() {
    int x = 10;
}
```

这类对象离开作用域时会自动销毁，不需要手动 `delete`。

## std::unique_ptr

```cpp
#include <iostream>
#include <memory>

int main() {
    std::unique_ptr<int> p(new int(42));
    std::cout << *p << std::endl;
}
```

`p` 离开作用域时，会自动释放资源。  
这比手写 `delete` 更不容易漏掉。

注意：C++11 里已经有 `std::unique_ptr`，但还没有标准版 `std::make_unique`；  
`std::make_unique` 是 C++14 才加入标准库的。

## std::shared_ptr

```cpp
#include <iostream>
#include <memory>

int main() {
    std::shared_ptr<int> p = std::make_shared<int>(100);
    std::cout << *p << std::endl;
}
```

多个 `shared_ptr` 可以共同拥有同一个对象，最后一个拥有者离开时对象才会释放。

不过也要注意：  
`shared_ptr` 不是“比 `unique_ptr` 更高级”，只有确实需要共享所有权时才应该用它。

---

# 和智能指针的关系

可以先把它们理解成这样：

| 方式 | 是否手动释放 | 适合场景 |
| --- | --- | --- |
| 原始 `new/delete` | 是 | 学习底层机制、实现底层组件、和旧代码交互 |
| `std::unique_ptr` | 否 | 明确单一拥有者 |
| `std::shared_ptr` | 否 | 多处共享同一个对象 |

在 C++11 代码里，一个很实用的经验是：

- 需要对象，先考虑直接定义对象
- 需要动态生命周期，再考虑智能指针
- 只有在确实要手动控制底层行为时，再直接写 `new/delete`

---

# 小结

在 C++11 背景下，`new/delete` 最重要的不是“背下来几种语法”，而是理解它们管理的是**对象生命周期**：

- `new` 会分配内存并构造对象
- `delete` 会析构对象并释放内存
- `new[]` 必须配 `delete[]`
- 手动管理容易出现泄漏、重复释放、悬空指针等问题
- 在很多日常代码里，C++11 更推荐优先使用 RAII 和智能指针

也就是说，`new/delete` 仍然必须会，但更应该把它们看成“底层能力”，而不是日常代码里的默认选择。