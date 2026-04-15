# 模板

C++ 模板是一种在编译期根据类型生成具体代码的机制。它和其他语言里的泛型有相似之处，但本质不同：

- 很多语言的泛型是 `统一规则 + 运行时或编译器约束`
- C++ 模板是 `按你给的类型，在编译期生成一份专属代码`

你不需要成为模板元编程大师，但你必须看懂模板代码，原因很现实：

- 标准库的容器和算法全部是模板（`std::vector<int>`、`std::map<std::string, int>`）
- 现代 C++ 的类型安全和泛型能力建立在模板上
- 线程池、网络库等基础设施代码大量使用模板来实现"透传任意参数"

----

# 函数模板

最简单的模板形式，让一个函数适用于不同类型：

```cpp
#include <iostream>

template <typename T>
T maxValue(T a, T b) {
    return (a > b) ? a : b;
}

int main() {
    std::cout << maxValue(3, 7) << std::endl;       // T = int
    std::cout << maxValue(2.5, 1.2) << std::endl;   // T = double
    return 0;
}
```

`template <typename T>` 声明了一个类型参数 `T`。  
编译器看到 `maxValue(3, 7)` 时，会推导出 `T = int`，然后生成一份 `int` 版本的 `maxValue`；  
看到 `maxValue(2.5, 1.2)` 时，会生成一份 `double` 版本。

## typename 和 class

在模板参数列表里，这两种写法基本等价

```cpp
template <typename T>   // 现代代码更常见
template <class T>      // 老代码里也很多
```

> 现代 C++ 更推荐 `typename`，因为它更明确地表达"这是一个类型参数"。本文统一使用 `typename`。

## 参数推导

编译器可以根据实参自动推导模板参数：

```cpp
maxValue(1, 2);       // T = int
maxValue(1.5, 2.5);   // T = double
```

但如果实参类型不一致，推导会失败：

```cpp
maxValue(1, 2.5);     // 编译错误：T 推导成 int 还是 double？
```

此时你可以显式指定类型，或者使用多个模板参数（后面会讲）：

```cpp
maxValue<double>(1, 2.5);  // 显式指定 T = double
```

## 多个参数

模板可以有不止一个类型参数，这在标准库中非常常见（例如 `std::map<Key, Value>`）：

```cpp
template <typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}

int main() {
    auto result = add(1, 2.5);   // T = int, U = double, 返回 double
    return 0;
}
```

这个例子里的 `decltype` 和尾置返回类型后面会详细讲。

----

# 类模板

类模板让你写出"结构相同、类型不同"的类：

```cpp
#include <iostream>
#include <string>

template <typename T>
class Box {
public:
    explicit Box(const T& value) : value_(value) {}

    void set(const T& value) { value_ = value; }
    const T& get() const { return value_; }

private:
    T value_;
};

int main() {
    Box<int> a(42);
    Box<std::string> b("hello");

    std::cout << a.get() << std::endl;   // 42
    std::cout << b.get() << std::endl;   // hello
    return 0;
}
```

`Box<int>` 和 `Box<std::string>` 是两个完全独立的类型，编译器会为它们分别生成代码。

> 标准库容器本质上就是类模板：

```cpp
std::vector<int>                  // 存 int 的动态数组
std::vector<std::string>          // 存 string 的动态数组
std::map<std::string, int>        // string → int 的有序映射
std::queue<std::function<void()>> // 存可调用对象的队列
```

容器的行为规则相同，但存储的元素类型不同。模板正好解决这种"规则统一、类型变化"的场景。

----

# 模板与引用

模板参数经常和引用组合使用，这在工程代码中是标准做法：

```cpp
template <typename T>
void print(const T& value) {
    std::cout << value << std::endl;
}
```

`const T&` 的好处：

- 避免大对象的拷贝
- 可以接受左值和右值
- 保持 const 正确性

你会在标准库和工程代码中反复看到这个模式。

> 模板的 `隐式约束`  
模板不要求类型继承某个基类，而是要求类型"支持某些操作"。  
上面的 `print` 函数要求 `T` 支持 `<<` 运算符，如果传入一个不支持的类型，编译器会在实例化时报错。  
这和 Go 的 interface 的显式约束不同——C++ 模板的约束是隐式的，错误信息也因此往往比较冗长。

----

# 模板代码放在头文件

你会经常看到模板的实现直接写在头文件（`.h` / `.hpp`）里，而不是分离到 `.cpp` 文件。这不是偷懒，而是必须的：

> 编译器在实例化模板时，需要看到完整的定义。

普通函数可以在头文件里声明、在 `.cpp` 里定义，因为编译器知道函数签名就够了。  
但模板是按类型生成代码的，编译器必须看到完整实现才能生成对应类型的版本。

!> 所以模板代码放在头文件里是 C++ 的常规做法，不要被 *头文件只放声明* 的规则框住。

----

# using 类型别名

C++11 引入了 `using` 语法来定义类型别名，替代老式的 `typedef`：

```cpp
// C++11 推荐
using IntVector = std::vector<int>;
using Callback = std::function<void()>;

// 等价的旧写法（不推荐）
typedef std::vector<int> IntVector;
typedef std::function<void()> Callback;
```

`using` 更清晰，而且支持模板别名（`typedef` 做不到）：

```cpp
template <typename T>
using Vec = std::vector<T>;

Vec<int> numbers;      // 等价于 std::vector<int>
Vec<std::string> names; // 等价于 std::vector<std::string>
```

在阅读工程代码时，`using` 经常出现在函数内部，用来简化复杂的类型名称：

```cpp
template <typename F, typename... Args>
auto submit(F&& f, Args&&... args) {
    using ReturnType = decltype(f(args...));
    // 后面多次使用 ReturnType，比每次写 decltype(f(args...)) 清晰得多
}
```

----

# auto 与模板

在模板代码中，展开后的类型往往很长。`auto` 可以帮你减少噪音：

```cpp
std::map<std::string, std::vector<int>> data;

// 不用 auto，迭代器类型极其冗长
std::map<std::string, std::vector<int>>::iterator it = data.begin();

// 用 auto
auto it = data.begin();
```

> `auto` 在模板代码中特别有价值，因为很多类型名称本身就包含模板参数，手写既冗长又容易出错。

----

# decltype

`auto` 让你在定义变量时省略类型，而 `decltype` 让你在 **不实际执行表达式** 的情况下拿到它的类型：

```cpp
int x = 42;
decltype(x) y = 10;   // y 的类型是 int，因为 x 是 int


// 更常见的场景是推导函数调用的返回类型：
int add(int a, int b) { return a + b; }

decltype(add(1, 2)) result = add(3, 4);  // result 是 int
```

`decltype(add(1, 2))` 不会真的调用 `add`，它只是让编译器推导出"如果调用 `add(1, 2)`，返回类型是什么"。

## 与模板结合

> `decltype` 在模板中解决了一个核心问题：当返回类型取决于参数类型时，你无法提前写死：

```cpp
template <typename T, typename U>
??? add(T a, U b) {     // 返回类型是什么？int + double = double，string + string = string
    return a + b;
}

// 用 `decltype` 就可以让编译器自动推导：

template <typename T, typename U>
decltype(a + b) add(T a, U b) {   // 编译错误！a 和 b 还没声明
    return a + b;
}
```

但这样写有个问题：在函数声明中，返回类型写在参数列表之前，此时 `a` 和 `b` 还没有声明，编译器找不到它们。  
这就需要尾置返回类型。

## 尾置返回类型

```cpp
// 普通函数的返回类型写在前面：
int add(int a, int b) { return a + b; }

// C++11 引入的尾置返回类型用 `auto` + `->` 把返回类型放到参数列表之后：
auto add(int a, int b) -> int {
    return a + b;
}

// 上面这个简单例子看不出优势。它真正有用的场景是配合 `decltype` 和模板：
template <typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}
```

因为返回类型写在 `->` 后面，此时 `a` 和 `b` 已经声明了，`decltype(a + b)` 可以正常使用。  
编译器会自动推导：`int + double` 返回 `double`，`string + string` 返回 `string`。

### 在工程代码中的典型出现

线程池的 `submit` 方法就使用了这个模式：

```cpp
template <typename F, typename... Args>
auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
```

`decltype(f(args...))` 推导出"调用 `f(args...)` 的返回类型"，  
再用 `std::future<...>` 包装，这样调用者就能通过 `future` 异步获取结果。

----

# std::function

> `std::function` 是一个通用的可调用对象包装器，可以持有任何签名匹配的可调用对象：  
> lambda、函数指针、`std::bind` 表达式等等

```cpp
#include <functional>
#include <iostream>

void hello() { std::cout << "hello" << std::endl; }

int main() {
    // 持有一个普通函数
    std::function<void()> f1 = hello;
    f1();

    // 持有一个 lambda
    std::function<int(int, int)> f2 = [](int a, int b) { return a + b; };
    std::cout << f2(3, 4) << std::endl;

    return 0;
}
```

`std::function<void()>` 表示"无参数、无返回值的可调用对象"。  
`std::function<int(int, int)>` 表示"接受两个 `int`、返回 `int` 的可调用对象"。

## 在线程池中使用

> 线程池的任务队列需要存储各种不同签名的任务，但队列的元素类型必须统一。  
> `std::function<void()>` 充当了"万能容器"：  
不管原始函数签名是什么，通过 `std::bind` 或 lambda 将参数绑定后，都变成了"无参无返回值"的可调用对象。

```cpp
std::queue<std::function<void()>> tasks;

// 不同签名的函数，绑定参数后都变成 void()
tasks.push([]{ std::cout << "task 1" << std::endl; });
tasks.push(std::bind(some_function, arg1, arg2));
```

----

# 变参模板

基础模板只有固定个数的类型参数。变参模板可以接受 **任意个数** 的类型参数：

```cpp
template <typename... Args>
void print(Args... args);
```

`typename... Args` 声明了一个**参数包**（parameter pack），表示"零个或多个类型参数"。`Args... args` 则是对应的函数参数包。

## 递归展开

C++11 中处理参数包的经典方式是递归：

```cpp
#include <iostream>

// 递归终止：只剩一个参数
template <typename T>
void print(T value) {
    std::cout << value << std::endl;
}

// 递归展开：取出第一个参数，剩余的继续递归
template <typename T, typename... Rest>
void print(T first, Rest... rest) {
    std::cout << first << " ";
    print(rest...);   // rest... 展开剩余参数
}

int main() {
    print(1, "hello", 3.14, 'x');
    // 输出: 1 hello 3.14 x
    return 0;
}
```

编译器会逐层展开：

1. `print(1, "hello", 3.14, 'x')` → 输出 `1`，调用 `print("hello", 3.14, 'x')`
2. `print("hello", 3.14, 'x')` → 输出 `hello`，调用 `print(3.14, 'x')`
3. `print(3.14, 'x')` → 输出 `3.14`，调用 `print('x')`
4. `print('x')` → 匹配单参数版本，输出 `x`

> 在工程代码中，变参模板最常见的用途不是递归打印，而是**透传任意参数**：  
> 让一个函数把参数原样传给另一个函数，不需要知道参数的个数和类型。后面的 `完美转发` 会展示这一点。

----

# 万能引用与完美转发

这是模板中最精妙也最实用的特性之一，也是理解线程池 `submit` 方法的关键。

## 万能引用

> 在模板中，`T&&` 有特殊含义——它不是普通的右值引用，而是**万能引用**，既能绑定左值也能绑定右值：

```cpp
template <typename T>
void wrapper(T&& arg) {
    // 传入左值 → T = Type&,  arg 是左值引用
    // 传入右值 → T = Type,   arg 是右值引用
}

int x = 10;
wrapper(x);    // T = int&,  arg 是 int&
wrapper(42);   // T = int,   arg 是 int&&
```

!> 只有在 **模板参数推导** 上下文中，`T&&` 才是万能引用。普通函数中的 `int&&` 就是纯粹的右值引用。

万能引用有个问题：*一旦参数有了名字，它在函数体内就变成了左值，不管它原来是左值还是右值：*

```cpp
template <typename T>
void wrapper(T&& arg) {
    // 不管 arg 原来是左值还是右值，
    // 这里 arg 有了名字，它就是左值
    process(arg);  // 永远按左值传递
}
```

如果 `process` 有针对右值的优化（比如移动语义），这个优化就完全失效了。

> `std::forward` 的作用就是 *记住参数原来是左值还是右值，并原样传递：*

```cpp
#include <utility>
#include <iostream>

void process(int& x)  { std::cout << "左值: " << x << std::endl; }
void process(int&& x) { std::cout << "右值: " << x << std::endl; }

template <typename T>
void wrapper(T&& arg) {
    process(std::forward<T>(arg));  // 保持原始的左值/右值属性
}

int main() {
    int x = 10;
    wrapper(x);    // 输出: 左值: 10
    wrapper(42);   // 输出: 右值: 42
    return 0;
}
```

## 完美转发

> 变参模板 + 万能引用 + std::forward  
这三者经常一起出现，构成了 **透传任意参数给内部函数，且不产生额外拷贝** 的标准模式：

```cpp
template <typename F, typename... Args>
auto invoke(F&& f, Args&&... args) {
    return f(std::forward<Args>(args)...);
}
```

拆解这行代码：

- `F&&`：万能引用，接受任意可调用对象
- `Args&&...`：万能引用的参数包，接受任意数量和类型的参数
- `std::forward<F>(f)`：保持 `f` 的左值/右值属性
- `std::forward<Args>(args)...`：逐个保持每个参数的左值/右值属性，`...` 让编译器对参数包中的每个参数都应用 `std::forward`

> 这个模式你会在线程池、`std::make_shared`、`emplace_back` 等大量标准库设施中看到。

----

# std::bind

`std::bind` 把一个函数和它的部分或全部参数绑定在一起，生成一个新的可调用对象：

```cpp
#include <functional>
#include <iostream>

int add(int a, int b) {
    return a + b;
}

int main() {
    // 把第一个参数固定为 10
    auto add10 = std::bind(add, 10, std::placeholders::_1);
    std::cout << add10(5) << std::endl;   // 输出: 15

    // 绑定所有参数，生成一个无参函数
    auto add_3_4 = std::bind(add, 3, 4);
    std::cout << add_3_4() << std::endl;  // 输出: 7

    return 0;
}
```

## 在线程池中的角色

线程池的核心需求：用户提交的函数签名各不相同，但工作线程只需要调用 `task()` 就行。  
`std::bind` 可以把任意函数 + 参数绑定成一个**无参可调用对象**：

```cpp
int compute(int a, int b) { return a + b; }

// 绑定后变成 void() 类型，可以塞进统一的任务队列
auto task = std::bind(compute, 3, 4);
task();  // 内部调用 compute(3, 4)
```

配合 `std::forward`，就能在绑定时保持参数的原始值类别：

```cpp
auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
```

----

# std::packaged_task

> `std::packaged_task` 把一个可调用对象包装起来，使其返回值可以通过 `std::future` 异步获取：

```cpp
#include <future>
#include <iostream>

int compute(int a, int b) { return a * b; }

int main() {
    // 包装函数，签名 int(int, int)
    std::packaged_task<int(int, int)> task(compute);

    // 获取关联的 future
    std::future<int> result = task.get_future();

    // 执行任务（可以在另一个线程中执行）
    task(5, 6);

    // 通过 future 获取结果
    std::cout << result.get() << std::endl;  // 输出: 30
    return 0;
}
```

当和 `std::bind` 配合时，可以把"带参数的函数"变成"无参函数"的 `packaged_task`：

```cpp
auto bound = std::bind(compute, 5, 6);

// packaged_task<int()> 表示"无参数、返回 int"
std::packaged_task<int()> task(std::move(bound));
task();   // 内部执行 compute(5, 6)
```

这正是线程池 `submit` 方法的核心手法。

----

# 实战：读懂线程池的 `submit`

现在把前面学到的所有特性拼在一起，来读 [高并发](advanced/concurrency.md) 章节中线程池的核心方法：

```cpp
template <typename F, typename... Args>
auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
{
    using ReturnType = decltype(f(args...));

    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<ReturnType> result = task->get_future();

    {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.emplace([task]() { (*task)(); });
    }
    cv_.notify_one();
    return result;
}
```

按前面学到的知识逐层拆解：

### 第 1 层：模板声明

```cpp
template <typename F, typename... Args>
```

- `F` — 可调用对象的类型（lambda、函数指针、functor）
- `Args...` — 参数包，接受零个或多个参数类型

### 第 2 层：签名与返回类型

```cpp
auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
```

- `F&&`、`Args&&...` — 万能引用，接受左值和右值
- `-> std::future<decltype(f(args...))>` — 尾置返回类型。`decltype(f(args...))` 推导出 `f` 的返回类型，再包进 `std::future` 里

### 第 3 层：类型别名

```cpp
using ReturnType = decltype(f(args...));
```

把推导出的返回类型存到 `ReturnType` 里，后面多次使用。

### 第 4 层：构造任务

```cpp
auto task = std::make_shared<std::packaged_task<ReturnType()>>(
    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
);
```

从内到外：

1. `std::forward<F>(f)` — 保持 `f` 的值类别
2. `std::forward<Args>(args)...` — 保持每个参数的值类别
3. `std::bind(...)` — 把函数和所有参数绑定成无参可调用对象
4. `std::packaged_task<ReturnType()>` — 包装成可以获取返回值的任务
5. `std::make_shared<...>(...)` — 放到堆上，用 `shared_ptr` 管理生命周期。因为任务要在 `submit` 返回后仍然存活（等待工作线程执行），不能放在栈上

### 第 5 层：入队与通知

```cpp
std::future<ReturnType> result = task->get_future();

{
    std::unique_lock<std::mutex> lock(mutex_);
    tasks_.emplace([task]() { (*task)(); });
}
cv_.notify_one();
return result;
```

1. 从 `packaged_task` 获取 `future`——调用者凭此取结果
2. 将任务包装成 `std::function<void()>` 放进队列。lambda 值捕获 `shared_ptr`，引用计数 +1，保证 task 不被释放
3. `cv_.notify_one()` 唤醒一个工作线程
4. 返回 `future` 给调用者

### 调用示例

```cpp
ThreadPool pool(4);

// 提交一个返回 int 的任务
auto future = pool.submit([](int a, int b) { return a + b; }, 3, 4);

// 获取结果（阻塞直到任务完成）
int result = future.get();  // result = 7
```

调用者不需要关心线程管理、锁、条件变量——这些全部被封装在线程池内部。

----

## 实参依赖查找概要

对调用 `f(a, b)`，除常规作用域查找外，编译器还会在 **实参所属类型所在的命名空间** 中检索候选，该机制称为 **实参依赖查找（ADL）**。

模板与泛型代码中常见影响：非成员运算符、按类型定制的 `swap` 与 `operator<<` 等往往依赖 ADL 才能被正确选中。  
在头文件中向 `namespace std` 注入特化或偏特化属于 **未定义行为**；仅允许在对应类型所在命名空间中定义非成员友元或特化策略。

与模板相关的 **编译期条件与断言** 的识读要点，见 [编译期约束概要](../note/compile-time.md)。

----

# 读模板代码的方法

当你遇到把多个模板特性堆在一起的代码时，建议按以下顺序拆解：

1. **先看模板参数**：有哪些类型参数？哪些是参数包？
2. **再看返回类型**：`decltype(...)` 推导的是什么表达式？
3. **再看 `std::forward`**：哪些参数被转发了？转发到了哪里？
4. **最后看整体流程**：输入是什么，经过了什么变换，输出是什么？

不要试图一次看懂所有尖括号。逐层拆解，从外到内，或者从内到外，每次只关注一层。

----

# 小结

模板是 C++ 泛型能力的核心，也是标准库大量能力的基础。本文覆盖的所有特性都来自 C++11，按它们在工程代码中的出现频率排列：

| 特性 | 用途 | 在线程池中的体现 |
|------|------|-----------------|
| 函数模板 / 类模板 | 类型参数化 | `template <typename F, typename... Args>` |
| `auto` / `decltype` | 类型推导 | `decltype(f(args...))` 推导返回类型 |
| 尾置返回类型 | 解决声明顺序 | `auto submit(...) -> std::future<...>` |
| `using` 别名 | 简化类型名 | `using ReturnType = decltype(...)` |
| `std::function` | 类型擦除 | 任务队列 `std::queue<std::function<void()>>` |
| 变参模板 | 接受任意参数 | `Args&&... args` |
| 万能引用 + `std::forward` | 完美转发 | `std::forward<Args>(args)...` |
| `std::bind` | 参数绑定 | 把函数 + 参数绑成无参对象 |
| `std::packaged_task` | 异步取值 | 包装任务，通过 `future` 返回结果 |
| 编译期断言与 `enable_if` 等 | 约束模板实参、按条件启用重载 | 见 [编译期约束概要](../note/compile-time.md) |

掌握这些，你就拥有了读懂现代 C++11 工程代码中模板部分的能力。接下来可以直接去读 [高并发](advanced/concurrency.md) 章节的线程池实现。
