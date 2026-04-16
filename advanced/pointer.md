# 指针

指针是 `C++` 最核心、也最容易让人踩坑的基础能力之一。  
旧时代学指针，重点常放在“地址”“解引用”“数组偏移”这些语法上；  
到了现代 `C++`，你还必须把“所有权”和“生命周期”一起带进来理解。

也就是说，今天看到一个指针时，最重要的问题已经不只是“它怎么用”，而是：
> 这个指针到底代表什么语义？

----

# 基本概念

指针保存的是地址。它不是对象本身，而是“指向对象”的一个值。

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

- `&value` 表示“取 `value` 的地址”
- `int* ptr` 表示“声明一个指向 `int` 的指针”
- `*ptr` 表示“沿着地址找到那个对象，再取出它的值”

如果你第一次进入 `C++` 时很容易在这里产生误判，因为 `C++` 里的指针可能同时承担多种角色：

- 它可能只是某个对象的地址
- 它可能表示“这个位置可能没有对象”
- 它可能被旧代码拿来表示“我负责释放这个资源”
- 它还可能只是某段连续内存的起点

所以学指针时，语法只是第一层。真正决定代码是否安全的，是你有没有把“地址”和“生命周期语义”分开理解。

----

# 空指针

旧代码里经常会看到：

```cpp
int* ptr = NULL;
```

现代 `C++` 推荐统一使用：

```cpp
int* ptr = nullptr;
```

原因不只是“写法更新了”，更重要的是 `nullptr` 有明确的空指针类型语义，而 `NULL` 往往只是整数 `0` 的宏定义。

这在重载解析里差别会非常明显：

```cpp
void f(int) {}
void f(int*) {}

f(0);        // 更像调用 f(int)
f(nullptr);  // 明确调用 f(int*)
```

所以从工程角度看，统一使用 `nullptr` 的价值在于：

- 代码意图更清楚
- 重载解析更安全
- **不再把“空指针”和“整数 0”混在一起**

----

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

这里 `nums` 在很多表达式中会退化成指向首元素的指针，所以：

- `nums` 可以看作首元素地址
- `*p` 访问第一个元素
- `*(p + 1)` 访问第二个元素

这也是为什么指针和数组在语法上看起来关系很近。但要注意，“关系很近”不等于“它们完全相同”。

例如：

- 数组本身有固定大小
- 指针只是一个地址值
- `sizeof(nums)` 和 `sizeof(p)` 往往不是一个含义

数组名退化成指针，是很多旧代码容易写出边界错误的原因之一。  
因为一旦只剩下一个指针，你通常已经失去了“这段内存到底有多长”的信息。

----

# const 与指针

```cpp
const int* p1 = nullptr;
int* const p2 = nullptr;
const int* const p3 = nullptr;
```

这是阅读项目代码时必须很熟的基础能力。可以先按 **`const` 修饰谁，谁就不能改** 来理解：

- `const int*`：不能通过指针修改它指向的数据
- `int* const`：指针本身不能再改指向
- `const int* const`：数据和指向都不能改

也可以这样口语化地读：

- `const int* p1`：`p1` 指向 **只读的 `int`**
- `int* const p2`：`p2` 是 **一个固定不变的指针**
- `const int* const p3`：`p3` 是 **一个固定不变、且指向只读数据的指针**

这个知识点看起来像语法细节，但它其实直接影响接口设计。因为它表达的是：

- 调用者能不能通过这个参数改数据
- 函数内部会不会改变指针指向
- 某个地址只是观察，还是允许修改内容

前面的 `常量` 一章已经先给过“只读边界”的第一层地图；到了这里，要把它和 `可空性`、`地址操作`、`所有权` 一起读，才更接近真实工程代码。

----

# 动态内存与生命周期

传统写法：

```cpp
int* p = new int(42);
delete p;

// 数组形式：
int* arr = new int[10];
delete[] arr;
```

这套语法本身没有错，但现代 `C++` 最大的经验教训之一就是：**手动管理动态内存太容易出问题**  
常见风险包括：

- 忘记 `delete`，导致内存泄漏
- 提前 `delete`，留下悬空指针
- 重复释放同一块内存
- 异常路径或早返回路径上资源没被正确释放

所以今天学习指针，不能只停留在“我会写 `new/delete`”。你更应该建立这样的直觉：
> 谁申请资源，谁负责释放，必须表达清楚；如果表达不清楚，代码迟早会出问题。

----

# 裸指针与所有权

现代 `C++` 并不是彻底不用裸指针，而是重新定义了它们最适合表达的语义。  
一个非常重要的观念是：

- 裸指针不应默认等于拥有资源
- 裸指针更适合作为观察者、可空借用或底层接口桥梁

也就是说，今天看到 `T*`，你更应该先想到下面这些可能：

- “这个对象可能存在，也可能为空”
- *我只是暂时观察它，不负责销毁*
- “我要和 C API 或底层接口对接”

而 **不应该** 下意识认为： “这个指针一定需要我 `delete`”  
*这和大量旧代码的习惯差异非常大，也是阅读现代项目时必须尽快建立的直觉。*

----

# 智能指针

`C++11` 最重要的指针升级之一，就是标准库智能指针。  
它们的价值不只是 `自动释放`，更重要的是把资源所有权写进类型里。

如果你还停留在“指针就是地址”这个阶段，那么学习智能指针时，最好再多建立一个视角：

- 谁拥有这个对象
- 谁负责销毁这个对象
- 其他代码拿到它时，是共享、借用，还是仅仅观察

智能指针本质上是在回答这些问题。

先记住一张最重要的选择表：

- `std::unique_ptr`：独占所有权，默认优先选择
- `std::shared_ptr`：共享所有权，只有确实需要共享生命周期时才用
- `std::weak_ptr`：观察 `shared_ptr` 管理的对象，但不参与拥有

## 基本思想

手写 `new/delete` 最大的问题，不只是代码麻烦，而是你很容易在这些场景里出错：

- 提前 `return`，忘记 `delete`
- 发生异常，清理代码没有执行
- 多处代码都以为自己负责释放，最后重复释放
- 没有人明确负责释放，最后泄漏

智能指针遵循的是 `RAII` 思想：

> 对象的生命周期绑定在作用域和类型上，离开作用域就自动清理资源。

所以很多现代 `C++` 代码里，你会看到：

- 很少直接写裸 `new`
- 更少直接写裸 `delete`
- 用类型表达“拥有关系”

真正重要的不是“我会不会写智能指针语法”，而是：

> 我能不能从类型上直接看出谁拥有对象、谁只是借用对象。

## unique_ptr

`std::unique_ptr` 表示独占所有权，也就是：

> 同一时刻只能有一个拥有者

当你明确知道“某个对象只应该有一个主人”时，它通常就是默认优先项。

### 创建方式

在 `C++11` 中，`std::unique_ptr` 通常要配合显式 `new` 创建：

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <utility>

class User {
public:
    explicit User(std::string name) : name_(std::move(name)) {}
    void print() const { std::cout << name_ << std::endl; }
private:
    std::string name_;
};

int main() {
    std::unique_ptr<User> user(new User("Alice"));
    user->print();

    if (user) {
        std::cout << "user is valid" << std::endl;
    }
    return 0;
}
```

这里最常见的几个操作是：

- `*p`：访问对象本身
- `p->member`：访问对象成员
- `if (p)`：判断是否为空

需要注意的是：

- `std::make_unique` 是 `C++14` 才加入标准库的
- 所以在严格的 `C++11` 语境下，`unique_ptr` 通常仍会写成 `std::unique_ptr<T>(new T(...))`

### 移动语义

`unique_ptr` 不能复制，只能移动。

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <utility>

int main() {
    std::unique_ptr<std::string> p1(new std::string("hello"));
    // auto p2 = p1;         // 错误：不能复制
    auto p2 = std::move(p1); // 正确：转移所有权

    if (!p1) {
        std::cout << "p1 is null" << std::endl;
    }

    std::cout << *p2 << std::endl;
    return 0;
}
```

这里 `std::move(p1)` 的含义不是“马上把字符串内容搬走”，而是：

- 把 `p1` 标记成一个可转移拥有权的对象
- 允许新的 `unique_ptr` 接管它原来拥有的资源

从设计角度看，这正是 `unique_ptr` 最重要的价值之一：

- 类型层面直接禁止错误复制
- 让“独占拥有”变成编译器可检查的规则

### 常见场景

`unique_ptr` 很适合下面这些情况：

- 类成员明确拥有某个资源
- 工厂函数返回新创建的对象
- 多态场景下通过基类指针拥有派生类对象
- 文件句柄、网络连接、任务对象这类明确独占的资源

例如工厂函数：

```cpp
#include <memory>
#include <string>
#include <utility>

class Config {
public:
    explicit Config(std::string path) : path_(std::move(path)) {
    }

private:
    std::string path_;
};

std::unique_ptr<Config> createConfig() {
    return std::unique_ptr<Config>(new Config("app.json"));
}
```

返回值会通过移动语义安全地转移所有权。

### 常用接口

`unique_ptr` 最常见的几个成员函数是：

- `get()`：获取内部裸指针，但不转移所有权
- `release()`：放弃所有权并返回裸指针
- `reset()`：释放当前对象，并可接管新对象
- `swap(other)`：交换两个 `unique_ptr`
- `operator bool`：判断是否非空

看一个更安全的例子：

```cpp
#include <iostream>
#include <memory>

int main() {
    std::unique_ptr<int> p(new int(42));

    int* raw = p.get();
    std::cout << *raw << std::endl;

    p.reset(new int(100));
    std::cout << *p << std::endl;

    int* released = p.release();
    std::cout << *released << std::endl;
    delete released;

    if (!p) {
        std::cout << "p is null" << std::endl;
    }

    return 0;
}
```

这里要特别注意：

- `get()` 只是临时拿到地址，资源仍归 `unique_ptr` 管
- `reset()` 后，旧对象已经释放，之前拿到的裸指针就不再安全
- `release()` 会把管理权交出来，但它本身不会 `delete`

也就是说：

> 一旦你从智能指针里拿到了裸指针，就必须非常清楚原拥有者后续是否还会释放或重置它。

### 参数传递

智能指针最容易写乱的地方之一，就是函数参数设计。

对 `unique_ptr` 可以先记这组经验规则：

- 函数要接管所有权：按值接收 `std::unique_ptr<T>`
- 函数只想使用对象：传 `T*`、`T&` 或 `const T&`
- 函数只是想观察智能指针本身状态：传 `const std::unique_ptr<T>&`

```cpp
#include <iostream>
#include <memory>
#include <utility>

class Task {
public:
    void run() const {
        std::cout << "run" << std::endl;
    }
};

void takeOwnership(std::unique_ptr<Task> task) {
    task->run();
}

void observeTask(const Task* task) {
    if (task != nullptr) {
        task->run();
    }
}

int main() {
    std::unique_ptr<Task> task(new Task());
    observeTask(task.get());
    takeOwnership(std::move(task));
    return 0;
}
```

这段代码表达得非常清楚：

- `observeTask` 只是借用对象
- `takeOwnership` 会接管对象

### 自定义删除器

这是 `unique_ptr` 在工程里很实用，但初学者容易忽视的能力。

很多资源不是用 `delete` 释放的，例如：

- `FILE*` 需要 `fclose`
- 某些第三方句柄需要专门的销毁函数

这时可以给 `unique_ptr` 指定删除器：

```cpp
#include <cstdio>
#include <memory>

struct FileCloser {
    void operator()(std::FILE* file) const {
        if (file != nullptr) {
            std::fclose(file);
        }
    }
};

int main() {
    std::unique_ptr<std::FILE, FileCloser> file(std::fopen("demo.txt", "w"));
    return 0;
}
```

这其实就是把一个 C 风格资源包装成了 `RAII` 对象。

## shared_ptr

`std::shared_ptr` 表示共享所有权，也就是：

> 多个拥有者共同管理同一个对象

它内部通常维护一个引用计数：

- 多一个 `shared_ptr`，计数加一
- 少一个 `shared_ptr`，计数减一
- 计数归零时，对象销毁

### 创建方式

`shared_ptr` 在 `C++11` 里通常优先使用 `std::make_shared`：

```cpp
#include <iostream>
#include <memory>

int main() {
    auto p1 = std::make_shared<int>(100);
    auto p2 = p1;

    std::cout << *p1 << std::endl;
    std::cout << p1.use_count() << std::endl;

    p2.reset();
    std::cout << p1.use_count() << std::endl;
    return 0;
}
```

相比显式 `new`，`make_shared` 的好处通常是：

- 写法更短
- 更不容易写乱所有权
- 在很多实现里分配更紧凑

### 适用场景

`shared_ptr` 适合下面这些真正存在共享生命周期的情况：

- 一个对象会被多个长期存在的模块共同持有
- 异步任务、回调或事件系统需要延长对象生命周期
- 某个框架接口本身就约定用共享所有权传递对象

但它不该被当成“懒得想所有权时的默认选择”。

因为它也有明显代价：

- 生命周期更难推理
- 引用计数有额外成本
- 更容易形成循环引用

所以更稳妥的顺序通常是：

1. 先问是否真的需要动态分配
2. 如果需要，先问是否只有一个明确拥有者
3. 只有答案是否定的，再考虑 `shared_ptr`

### 常用接口

`shared_ptr` 常见接口包括：

- 复制构造、复制赋值：共享同一对象
- `use_count()`：查看当前拥有者数量
- `get()`：获取裸指针，但不改变所有权
- `reset()`：放弃当前拥有关系
- `operator bool`：判断是否非空

这里最需要补的一条直觉是：

> `shared_ptr` 表达的是“共享拥有”，不是“任何地方都能随便拿来传一份”。

如果一个函数只是读取对象，很多时候更好的参数仍然是：

- `const T&`
- `const T*`

而不是强行写成 `const std::shared_ptr<T>&`。

### 常见错误

不要从同一个裸指针独立构造多个 `shared_ptr`，否则会导致重复释放。

```cpp
int* raw = new int(10);

std::shared_ptr<int> p1(raw);
std::shared_ptr<int> p2(raw);  // 错误
```

正确做法应该是：

```cpp
auto p1 = std::make_shared<int>(10);
auto p2 = p1;
```

这两段代码的根本区别在于：

- 前者创建了两套互不相干的拥有关系
- 后者是在同一套拥有关系上增加新的拥有者

### 参数传递

对 `shared_ptr`，可以先记下面这组经验规则：

- 函数要保留一份共享拥有权：按值接收 `std::shared_ptr<T>`
- 函数只在调用期间使用对象：优先传 `T&`、`const T&`、`T*`
- 函数只是想读取共享指针状态：传 `const std::shared_ptr<T>&`

看一个例子：

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <utility>

class Document {
public:
    explicit Document(std::string name) : name_(std::move(name)) {
    }

    void print() const {
        std::cout << name_ << std::endl;
    }

private:
    std::string name_;
};

void keepDocument(std::shared_ptr<Document> doc) {
    doc->print();
}

void preview(const Document& doc) {
    doc.print();
}

int main() {
    auto doc = std::make_shared<Document>("report.pdf");
    preview(*doc);
    keepDocument(doc);
    std::cout << doc.use_count() << std::endl;
    return 0;
}
```

这里：

- `keepDocument` 会额外保留一份共享拥有权
- `preview` 只是借用对象本身

这种区分对阅读真实工程代码非常重要。

## weak_ptr

`std::weak_ptr` 的意义，不在于“再来一种指针”，而在于它专门解决共享所有权里的观察问题。

它通常和 `shared_ptr` 配合使用，表示：

- 我知道这个对象由共享所有权管理
- 我想观察它
- 但我不想成为拥有者之一

### 基础语义

`weak_ptr` 不会增加引用计数，也不会阻止对象被销毁。

它不能直接像 `shared_ptr` 那样解引用，通常要先通过 `lock()` 拿到一个临时的 `shared_ptr`：

```cpp
#include <iostream>
#include <memory>

int main() {
    auto sp = std::make_shared<int>(42);
    std::weak_ptr<int> wp = sp;

    if (auto locked = wp.lock()) {
        std::cout << *locked << std::endl;
    }

    sp.reset();

    if (wp.expired()) {
        std::cout << "object destroyed" << std::endl;
    }

    return 0;
}
```

这里最关键的动作是 `lock()`：

- 如果对象还活着，返回新的 `shared_ptr`
- 如果对象已经销毁，返回空结果

### 循环引用

`weak_ptr` 最典型的用途，是打破循环引用。

例如两个对象彼此都用 `shared_ptr` 保存对方：

```cpp
class A {
public:
    std::shared_ptr<B> b_;
};

class B {
public:
    std::shared_ptr<A> a_;
};
```

这会导致：

- `A` 拥有 `B`
- `B` 又拥有 `A`
- 两边计数都降不到 0
- 对象永远不释放

这时通常应让其中一边改成 `weak_ptr`。

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <utility>

struct Node {
    std::string name;
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev;

    explicit Node(std::string nodeName) : name(std::move(nodeName)) {
        std::cout << name << " created" << std::endl;
    }

    ~Node() {
        std::cout << name << " destroyed" << std::endl;
    }
};

int main() {
    auto first = std::make_shared<Node>("first");
    auto second = std::make_shared<Node>("second");

    first->next = second;
    second->prev = first;

    if (auto prev = second->prev.lock()) {
        std::cout << "second.prev -> " << prev->name << std::endl;
    }

    first.reset();
    second.reset();
    return 0;
}
```

这里 `prev` 更像“回看上一个节点的观察引用”，而不是“拥有上一个节点”。

### 常用接口

`weak_ptr` 常用接口包括：

- `lock()`：尝试拿到临时 `shared_ptr`
- `expired()`：判断对象是否已经被销毁
- `reset()`：清空当前观察关系
- `use_count()`：查看当前共享对象拥有者数量

对初学者来说，先记住一句就很有用：

> `weak_ptr` 是共享对象的非拥有观察者，也是打破循环引用的重要工具。

## 设计顺序

写代码时，如果你不确定该用哪一种智能指针，可以按这个顺序思考：

1. 这个对象是否真的需要动态分配
2. 如果需要，是否只有一个明确拥有者
3. 如果不是，是否真的存在多个地方共享生命周期
4. 如果只是观察共享对象，是否应该用 `weak_ptr`

很多设计问题，最终都能化成一句话：

> 先把“谁拥有它”想清楚，再决定“该用什么指针”。

## 常见误区

- `shared_ptr` 不是“更高级的默认指针”，默认仍然应该先考虑 `unique_ptr`
- 智能指针管理的是“对象生命周期”，不是“所有并发访问都自动安全”
- 不要把同一个裸指针同时交给多个智能指针独立管理
- 既然已经交给智能指针管理，就不要再手写 `delete`
- 在严格的 `C++11` 语境里，`unique_ptr` 通常仍需显式 `new`
- 需要非拥有访问时，很多时候 `T*` 或 `T&` 就够了，不一定非要上传递智能指针

----

# 与 GC 语言的差异

如果你来自带垃圾回收的语言，可以先建立这样一个对比：

- GC 语言里，你更多关心 `还有没有引用`
- `C++` 里，你更需要关心 `谁拥有、谁借用、谁负责销毁`

这不是单纯的内存释放问题，而是整个接口设计方式都会受到影响。

例如在 GC 语言里，代码往往默认假设对象最终会被回收；  
而在 `C++` 里，类型设计本身就常常在表达生命周期边界。  
也正因为如此，现代 `C++` 才会特别强调用智能指针、容器和值语义把资源关系写清楚。

----

# 综合示例

下面这个例子更接近工程语境：

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

int main() {
    std::unique_ptr<Logger> logger(new Logger());
    UserService service(std::move(logger));
    service.createUser("Alice");
    return 0;
} 
```

示例输出：

```text
Alice
```

这里读代码时，你能很快看出：

- `UserService` 拥有 `Logger`
- 拥有关系通过 `std::unique_ptr` 表达
- `Logger` 的生命周期会随着 `UserService` 一起结束

这就是现代 `C++` 一个很重要的价值：类型本身就在表达设计意图。

再看一个更轻量的非拥有场景：

```cpp
#include <iostream>

void print(const int* p) {
    if (p != nullptr) {
        std::cout << *p << std::endl;
    } else {
        std::cout << "pointer is null" << std::endl;
    }
}

int main() {
    int value = 10;
    print(&value);
    print(nullptr);
    return 0;
}
```

示例输出：

```text
10
pointer is null
```

这里的 `const int*` 更像“可空的只读观察者”，它并不拥有那个 `int`，只是暂时访问一下。

把这两个例子放在一起看，你会更容易建立一个很实用的阅读顺序：

1. 先看这个指针是否拥有资源
2. 再看它是否允许为空
3. 最后看它是否允许修改目标对象

----

# 推荐实践

- 空指针统一使用 `nullptr`。
- 一看到指针就先问“这个指针是否拥有资源”。
- 能不用裸 `new/delete` 就不用。
- 独占所有权优先使用 `std::unique_ptr`。
- 共享所有权有明确需求时再用 `std::shared_ptr`。
- 观察共享对象但不拥有时考虑 `std::weak_ptr`。
- 与 C API 或非拥有关系交互时，裸指针仍然合理，但语义一定要想清楚。

----

# 小结

指针不只是“地址”的语法问题，更是“所有权、生命周期、可空性”三件事的组合表达。  
阅读或编写指针相关代码时，先判断它是否拥有资源，再判断是否允许为空，最后再看是否允许修改对象，这样通常会更稳、更不容易出错。
