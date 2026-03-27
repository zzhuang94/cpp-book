# 编码规范

这一章不是为了制定一套苛刻的团队规范，而是帮助你在接手 `C++11` 项目时，尽快形成一套“现代、稳定、可维护”的默认写法。很多问题并不是编译器会不会报错，而是代码是否容易读、容易改、容易避免低级错误。

## 总原则

可以先记住一句话：

> 优先写意图清晰、所有权明确、生命周期稳定、能让编译器帮你发现错误的代码。

这几乎是现代 `C++` 编码规范背后的共同逻辑。

## 1. 优先使用 C++11 写法，而不是停留在 C++98 习惯

比如：

- 用 `nullptr`，不要再用 `NULL`
- 用 `auto` 处理冗长类型，而不是机械拼完整模板名
- 用范围 `for` 遍历容器，而不是优先写迭代器样板
- 用 `using`，而不是优先写 `typedef`
- 用 `enum class`，而不是裸 `enum`
- 用智能指针表达所有权，而不是手动 `new/delete`

### 例子：`NULL` vs `nullptr`

不推荐：

```cpp
int* p = NULL;
```

推荐：

```cpp
int* p = nullptr;
```

## 2. 定义变量时立即初始化

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

## 3. 优先使用花括号初始化

```cpp
int a{10};
std::vector<int> values{1, 2, 3};
```

它的价值不只是统一风格，还包括更明确的初始化语义。

但也要注意，某些类型在不同初始化方式下构造函数匹配可能不同，遇到复杂类型时要理解其构造规则。

## 4. `auto` 要服务于可读性

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

## 5. 能用 `const` 就用 `const`

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

## 6. 参数传递遵循清晰规则

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

## 7. 不要轻易裸写 `new` 和 `delete`

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

## 8. 裸指针默认表示“非拥有关系”

现代 `C++` 更推荐把裸指针理解为：

- 观察者
- 可为空引用
- 与底层接口交互的桥梁

而不是“自动意味着负责释放资源”。

这能极大降低阅读代码时对所有权的误判。

## 9. 范围 `for` 优先于手写迭代器循环

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

## 10. 重写虚函数时始终写 `override`

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

## 11. 能 `=default`、`=delete` 就显式表达

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

## 12. 命名要反映角色，而不是只反映类型

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

## 13. 函数尽量短，职责尽量单一

如果一个函数同时做：

- 参数校验
- 业务逻辑
- 日志输出
- 文件写入
- 错误恢复

那它大概率已经太重了。

更推荐把逻辑拆成几个命名良好的小函数。现代 `C++` 有 `lambda`、局部对象和标准库算法，拆分成本比过去更低。

## 14. 优先使用标准库，而不是重复造轮子

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

## 15. 注释解释“为什么”，而不是“代码在做什么”

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

## 16. 头文件和实现文件保持清晰边界

虽然本文档不展开大型工程组织，但至少建议遵守：

- 头文件放声明
- 源文件放实现
- 头文件避免无必要的 `using namespace`
- 头文件尽量减少不必要依赖

这会直接影响编译速度、依赖关系和可维护性。

## 17. 保持一致，比局部“更高级”更重要

真实项目里，编码规范的价值很大程度上来自一致性。即使一种写法不是理论上的绝对最优，只要：

- 语义清晰
- 错误率低
- 团队一致

它通常就比“每个人都写自己最喜欢的现代花样”更有价值。

## 一个综合示例

下面这段代码把多个规范放在一起：

```cpp
#include <iostream>
#include <memory>
#include <string>
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

## 小结

好的 `C++11` 编码规范，本质上是在减少歧义、减少资源错误、减少阅读负担。你不需要一开始就记住所有规则，但至少要尽快把下面这些变成默认直觉：

- `nullptr` 替代 `NULL`
- 智能指针替代手写资源释放
- `override` 明确接口意图
- `const` 表达只读约束
- `auto`、范围 `for`、统一初始化用于提升可读性

当这些习惯稳定下来后，你读 `C++11` 项目代码会顺畅很多，写出来的代码也会更像现代 `C++`，而不是停留在旧时代经验的延续。
