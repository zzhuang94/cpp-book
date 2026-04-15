# 编码规范

这一章不是为了制定一套苛刻的团队规范，而是帮助你在接手 `C++11` 项目时，尽快形成一套“现代、稳定、可维护”的默认写法。很多问题并不是编译器会不会报错，而是代码是否容易读、容易改、容易避免低级错误。

# 总原则

可以先记住一句话：

> 优先写意图清晰、所有权明确、生命周期稳定、能让编译器帮你发现错误的代码。

这几乎是现代 `C++` 编码规范背后的共同逻辑。

# 优先 C++11 写法

比如：

- 用 `nullptr`，不要再用 `NULL`
- 用 `auto` 处理冗长类型，而不是机械拼完整模板名
- 用范围 `for` 遍历容器，而不是优先写迭代器样板
- 用 `using`，而不是优先写 `typedef`
- 用 `enum class`，而不是裸 `enum`
- 用智能指针表达所有权，而不是手动 `new/delete`

## `NULL` 与 `nullptr`

不推荐：

```cpp
int* p = NULL;
```

推荐：

```cpp
int* p = nullptr;
```

# 立即初始化

不推荐：

```cpp
int count;
count = 10;
```

推荐：

```cpp
int count = 10;
```

更推荐统一初始化：

```cpp
int count{10};
```

这样做的好处是：

- 降低未初始化变量风险
- 让变量语义更集中
- 避免某些窄化转换

# 花括号初始化

```cpp
int a{10};
std::vector<int> values{1, 2, 3};
```

它的价值不只是统一风格，还包括更明确的初始化语义。

但也要注意，某些类型在不同初始化方式下构造函数匹配可能不同，遇到复杂类型时要理解其构造规则。

# `auto` 与可读性

`auto` 不是“写得越多越现代”，而是“在类型冗长、推导清晰时使用”。

推荐：

```cpp
std::vector<std::string> names{"a", "b", "c"};
auto it = names.begin();
```

推荐：

```cpp
auto handler = [](int x) { return x * 2; };
```

谨慎使用：

```cpp
auto value = getSomethingComplicated();
```

如果右侧表达式语义模糊，`auto` 可能反而让读者不知道类型是什么。

# `const` 优先

`const` 是现代 `C++` 最重要的自说明机制之一。

推荐：

```cpp
void print(const std::string& name);
```

推荐：

```cpp
class User {
public:
    const std::string& name() const {
        return name_;
    }

private:
    std::string name_;
};
```

这能帮助你表达：

- 参数不会被修改
- 成员函数不会修改对象状态
- 某个对象初始化后不应变化

# 参数传递

可以先采用下面这套简单规则：

- 小型基础类型按值传递：`int`、`double`、`bool`
- 只读大对象按 `const T&`
- 需要修改调用方对象时用 `T&`
- 需要接管临时资源时考虑 `T&&`

例如：

```cpp
void setAge(int age);
void printName(const std::string& name);
void normalize(std::vector<int>& values);
void setBuffer(std::string&& buffer);
```

# 避免裸写 `new/delete`

这是接手现代 `C++` 项目时最重要的规范之一。

不推荐：

```cpp
Widget* w = new Widget();
delete w;
```

推荐：

```cpp
std::unique_ptr<Widget> w(new Widget());
```

如果资源是独占所有权，优先 `std::unique_ptr`。只有在确实需要共享所有权时，再使用 `std::shared_ptr`。

# 裸指针语义

现代 `C++` 更推荐把裸指针理解为：

- 观察者
- 可为空引用
- 与底层接口交互的桥梁

而不是“自动意味着负责释放资源”。

这能极大降低阅读代码时对所有权的误判。

# 范围 `for`

不推荐把下面这种写法作为默认风格：

```cpp
for (std::vector<int>::iterator it = values.begin(); it != values.end(); ++it) {
    std::cout << *it << std::endl;
}
```

更推荐：

```cpp
for (const auto& value : values) {
    std::cout << value << std::endl;
}
```

遍历时有一个实用约定：

- 只读：`const auto&`
- 要修改元素：`auto&`
- 明确需要拷贝：`auto`

# `override`

推荐：

```cpp
class Base {
public:
    virtual void run() const = 0;
};

class Derived : public Base {
public:
    void run() const override {
    }
};
```

不要省略 `override`，因为它能让编译器替你发现签名不匹配问题。

# `=default` 与 `=delete`

推荐：

```cpp
class FileHandle {
public:
    FileHandle() = default;
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
};
```

好处是：

- 读代码时意图非常直接
- 不必靠注释说明“这个类不允许复制”
- 编译器会强制执行你的设计

# 语义化命名

不推荐：

```cpp
std::vector<int> v;
std::string s;
```

推荐：

```cpp
std::vector<int> scores;
std::string userName;
```

尤其在 `auto` 增多后，变量名承载的语义会更加重要。

# 单一职责

如果一个函数同时做：

- 参数校验
- 业务逻辑
- 日志输出
- 文件写入
- 错误恢复

那它大概率已经太重了。

更推荐把逻辑拆成几个命名良好的小函数。现代 `C++` 有 `lambda`、局部对象和标准库算法，拆分成本比过去更低。

# 标准库优先

接手项目时很容易看到历史代码自己实现：

- 字符串拼接工具
- 动态数组
- 链表容器
- 时间工具
- 简单智能指针

如果没有明确性能或兼容性约束，优先使用标准库提供的能力：

- `std::string`
- `std::vector`
- `std::array`
- `std::unordered_map`
- `std::chrono`
- `std::unique_ptr`

# 注释意图

不推荐：

```cpp
// i 加 1
++i;
```

更有价值的注释通常是：

```cpp
// 这里必须先更新状态，再通知观察者，否则回调里会读取到旧值
updateState();
notifyObservers();
```

代码本身应该尽量清楚，注释主要用于解释设计意图、约束条件和非显然原因。

# 头文件边界

虽然本文档不展开大型工程组织，但至少建议遵守：

- 头文件放声明
- 源文件放实现
- 头文件避免无必要的 `using namespace`
- 头文件尽量减少不必要依赖

这会直接影响编译速度、依赖关系和可维护性。

# 保持一致

真实项目里，编码规范的价值很大程度上来自一致性。即使一种写法不是理论上的绝对最优，只要：

- 语义清晰
- 错误率低
- 团队一致

它通常就比“每个人都写自己最喜欢的现代花样”更有价值。

# 综合示例

下面这段代码把多个规范放在一起：

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Printer {
public:
    virtual ~Printer() = default;
    virtual void print(const std::string& text) const = 0;
};

class ConsolePrinter : public Printer {
public:
    void print(const std::string& text) const override {
        std::cout << text << std::endl;
    }
};

class UserService {
public:
    explicit UserService(std::unique_ptr<Printer> printer)
        : printer_(std::move(printer)) {
    }

    void printUsers(const std::vector<std::string>& users) const {
        for (const auto& user : users) {
            printer_->print(user);
        }
    }

private:
    std::unique_ptr<Printer> printer_;
};

int main() {
    std::vector<std::string> users{"alice", "bob", "carol"};
    UserService service(std::unique_ptr<Printer>(new ConsolePrinter()));
    service.printUsers(users);
    return 0;
}
```

这里体现了：

- 使用智能指针表达所有权
- 接口类提供虚析构和纯虚函数
- 实现类重写时使用 `override`
- 大对象参数使用 `const T&`
- 容器遍历使用 `const auto&`
- 命名围绕语义而不是单字母

# 小结

好的 `C++11` 编码规范，本质上是在减少歧义、减少资源错误、减少阅读负担。你不需要一开始就记住所有规则，但至少要尽快把下面这些变成默认直觉：

- `nullptr` 替代 `NULL`
- 智能指针替代手写资源释放
- `override` 明确接口意图
- `const` 表达只读约束
- `auto`、范围 `for`、统一初始化用于提升可读性

当这些习惯稳定下来后，你读 `C++11` 项目代码会顺畅很多，写出来的代码也会更像现代 `C++`，而不是停留在旧时代经验的延续。

## 面向其它语言开发者的补充说明

如果你来自 `Go`、`PHP`、`JS`、`TS`，第一次看 `C++` 编码规范时，容易把它理解成“语法偏好列表”。但在 `C++` 里，很多规范背后其实都对应着真实风险：

- 不统一使用 `nullptr`，可能引出重载歧义。
- 随意裸写 `new/delete`，可能引出泄漏和释放时机问题。
- 不写 `override`，可能导致你以为自己覆盖了虚函数，实际却没有。
- 不重视 `const`，会让接口边界模糊。

所以这章不是在追求形式统一，而是在帮助你建立“默认更稳的写法”。

## 头文件规范

对 `C++` 项目来说，头文件是非常高频的协作边界。至少应养成这些习惯：

- 头文件主要放声明，源文件主要放实现。
- 头文件加保护，避免重复包含。
- 头文件中避免写 `using namespace std;`。
- 头文件尽量包含最少依赖。
- 如果只需要声明关系，优先考虑前向声明。

这些规则会直接影响：

- 编译速度
- 依赖复杂度
- 名字冲突概率
- 项目整体可维护性

## 所有权表达风格

现代 `C++` 最值得形成统一规范的一点，就是所有权表达。

推荐的默认顺序通常是：

- 栈对象优先于堆对象
- 容器优先于手工数组
- `std::unique_ptr` 优先于裸 `new/delete`
- 确有共享需求时再用 `std::shared_ptr`
- 裸指针更多表示非拥有关系

统一这一点的收益非常大，因为团队成员读代码时能更快判断：

- 谁负责释放资源
- 某个指针是不是只是借用
- 生命周期大致如何流动

## 参数传递风格

实际项目里，函数签名风格如果不统一，很快就会让代码变难读。一个很实用的默认规则集是：

- 小型基础类型按值传递
- 只读大对象按 `const T&`
- 需要修改调用方对象时用 `T&`
- 可为空借用用 `T*`
- 所有权转移用值或智能指针，并配合移动语义

这不是绝对真理，但足够成为大多数业务代码的稳妥起点。

## 容器和算法风格

为了让代码更像现代 `C++`，建议尽量减少“自己重新发明一套容器使用方式”的倾向。

推荐习惯：

- 默认先考虑 `std::vector`
- 遍历容器优先范围 `for`
- 只读遍历优先 `const auto&`
- 能用算法库表达，就不要手写过长循环
- 需要关联查找时优先考虑 `map` / `unordered_map`

这类风格统一后，代码会更容易被团队成员快速读懂。

## 类设计风格

类设计在现代 `C++11` 中也应有一些稳定规范：

- 成员尽量在初始化列表中初始化
- 简单默认行为显式用 `=default`
- 不允许的复制或赋值显式用 `=delete`
- 作为多态基类的类型通常提供虚析构函数
- 只要是覆盖虚函数，就加 `override`

这些写法的共同价值是：把设计意图显式写出来，而不是藏在注释或隐性约定里。

## 错误处理风格

不同团队对异常的使用范围会有差异，但无论使用哪种策略，都建议保持一致：

- 哪类错误用返回值
- 哪类错误用异常
- 在模块边界如何转换错误
- 析构函数中如何处理失败

风格不统一时，代码最容易出现的问题是：有的地方层层返回错误码，有的地方直接抛异常，最终让控制流很难推理。

## 注释风格

高质量注释不应该重复代码表面含义，而应优先解释：

- 为什么要这么做
- 这里的边界条件是什么
- 有什么历史兼容性约束
- 哪些行为不是表面代码就能看出来的

例如：

不推荐：

```cpp
// i 加 1
++i;
```

推荐：

```cpp
// 必须先更新本地状态，再触发回调，否则观察者会读到旧值
updateState();
notifyObservers();
```

## 命名风格建议

命名规范团队之间可以不同，但好的命名通常有共同特征：

- 能表达业务语义
- 避免无意义缩写
- 与作用域和生命周期相匹配
- 与类型语义不冲突

例如：

- `userName` 通常比 `s` 更好
- `isReady` 能立即表达布尔语义
- `printer_` 这样的成员后缀风格只要团队统一即可

## 一致性比个人偏好更重要

现实项目中，很少存在一个“理论上完美”的风格答案。更重要的是：

- 团队能否在大方向上统一
- 新成员是否能快速读懂代码
- 是否能显著减少低级错误

因此，一致性本身就是一种生产力。

## 代码评审时值得重点看的点

如果你以后参与 `C++` 代码评审，下面这些点通常比“排版是不是我最喜欢的样子”更值得先看：

- 是否明确表达了资源所有权
- 是否有裸 `new/delete`
- 是否有明显的生命周期风险
- 参数传递方式是否合理
- 是否遗漏 `const`、`override`、虚析构
- 是否有不必要的复杂重载和隐式转换
- 容器和算法选择是否合理

这些地方往往比纯格式更影响长期质量。

## 常见反模式

以下写法在现代 `C++11` 项目里通常值得警惕：

- 头文件里大量 `using namespace`
- 随处裸 `new/delete`
- 为了少写几个字，到处滥用 `auto`
- 为了“更抽象”而过早设计复杂继承层次
- 通过注释说明“不允许复制”，却不使用 `=delete`
- 把可空关系写成普通引用，让语义含糊

这些问题的共同特点是：短期好像没什么，长期会显著增加维护成本。

## 一个更完整的风格示例

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Logger {
public:
    virtual ~Logger() = default;
    virtual void write(const std::string& text) const = 0;
};

class ConsoleLogger : public Logger {
public:
    void write(const std::string& text) const override {
        std::cout << text << std::endl;
    }
};

class UserService {
public:
    explicit UserService(std::unique_ptr<Logger> logger)
        : logger_(std::move(logger)) {
    }

    void printUsers(const std::vector<std::string>& users) const {
        for (const auto& user : users) {
            logger_->write(user);
        }
    }

private:
    std::unique_ptr<Logger> logger_;
};

int main() {
    std::vector<std::string> users{"alice", "bob", "carol"};
    UserService service(std::unique_ptr<Logger>(new ConsoleLogger()));
    service.printUsers(users);
    return 0;
}
```

这个例子里可以同时看到：

- 接口边界通过抽象类表达
- 虚析构和 `override` 保证多态安全
- 所有权通过 `unique_ptr` 表达
- 只读参数使用 `const T&`
- 遍历时使用 `const auto&`
- 命名尽量表达语义

## 给自己的快速检查清单

写完一段 `C++11` 代码后，你可以快速问自己：

1. 我是否把资源所有权写清楚了？
2. 这里是否真的需要裸指针？
3. 这个函数参数形式是否表达了真实语义？
4. 多态接口是否有虚析构和 `override`？
5. 这里能否用标准库容器或算法替代手写样板？
6. 变量是否在定义时初始化？
7. 有没有本可以交给编译器检查的约束，还停留在注释里？

如果这几个问题大都能回答得比较坚定，代码风格通常就已经比较接近现代 `C++11` 了。
