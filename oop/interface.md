# 接口

`C++` 没有单独的 `interface` 关键字，但它并不缺少接口设计能力。相反，`C++` 往往把“接口”拆开表达为几件事：

- 用抽象基类定义能力边界
- 用纯虚函数定义必须实现的行为
- 用组合和依赖注入连接具体实现

到了 `C++11`，`override`、`final` 和智能指针让这套写法更安全，也更适合工程实践。

----

# 抽象类

最典型的接口写法是抽象基类：

```cpp
#include <iostream>
#include <string>

class Drawable {
public:
    virtual ~Drawable() = default;
    virtual void draw() const = 0;
};

class Button : public Drawable {
public:
    void draw() const override {
        std::cout << "draw button" << std::endl;
    }
};
```

带有纯虚函数的类不能直接实例化，因为它表达的是“能力边界”，而不是一个完整对象。

## 抽象类到底在表达什么

当你写出 `virtual void draw() const = 0;`，本质上是在说：

- 任何具体实现都必须提供 `draw()`
- 调用方只依赖“会画图”这个能力
- 基类本身不关心具体怎么画

这就是接口设计的核心价值：**让调用方依赖能力，而不是依赖具体实现细节。**

## “接口类”在 `C++` 里通常长什么样

虽然语言里没有 `interface` 关键字，但工程里经常会把下面这种类当成“纯接口类”：

```cpp
class Logger {
public:
    virtual ~Logger() = default;
    virtual void write(const std::string& text) = 0;
};
```

它通常有这些特点：

- 主要由纯虚函数组成
- 没有或几乎没有数据成员
- 重点在于定义能力边界，而不是提供状态

这类写法本质上就是 `C++` 版本的接口。

----

# 接口类与抽象基类

很多资料会把这两个词混着说，但它们其实可以稍微区分一下。

- 接口类：更偏向“只定义能力，不提供实现”
- 抽象基类：除了抽象接口，也可能提供一部分默认实现或公共工具逻辑

例如下面更像纯接口类：

```cpp
class Storage {
public:
    virtual ~Storage() = default;
    virtual void save(const std::string& data) = 0;
};
```

而下面更像抽象基类：

```cpp
#include <iostream>
#include <string>

class Logger {
public:
    virtual ~Logger() = default;

    void log(const std::string& text) {
        std::cout << "[log] ";
        write(text);
    }

    virtual void write(const std::string& text) = 0;
};
```

这里 `Logger` 仍然是抽象类，但它已经提供了一部分共享逻辑。

## 为什么要分这个感觉

因为这会影响你的设计判断：

- 如果你只是想定义“必须提供哪些能力”，接口类更合适
- 如果你还想复用一部分稳定流程或公共逻辑，抽象基类更合适

但无论哪一种，都不应把基类做成“大而全的万能父类”

----

# 虚析构函数

接口常通过基类指针或引用使用：

```cpp
Drawable* p = new Button();
delete p;
```

如果析构函数不是虚的，就可能只执行基类析构，导致派生类资源没有被正确释放，进而出现资源泄漏甚至未定义行为。

## 为什么接口基类几乎总要有虚析构

只要一个类打算被“当成基类接口”使用，就应当优先考虑虚析构。原因很简单：

- 调用方通常拿到的是基类指针或基类引用
- 真正被销毁的往往是派生类对象
- 没有虚析构时，销毁行为可能不完整

很多时候，即使基类自己没有资源，也仍然应该写：

```cpp
virtual ~Base() = default;
```

因为这不是在表达“基类自己需要复杂析构”，而是在表达“这个类型允许被多态销毁”。

----

# 接口编程

接口的核心价值在于“调用方依赖能力，而不是依赖具体实现”。

```cpp
#include <iostream>
#include <string>

class Logger {
public:
    virtual ~Logger() = default;
    virtual void write(const std::string& text) = 0;
};

class ConsoleLogger : public Logger {
public:
    void write(const std::string& text) override {
        std::cout << text << std::endl;
    }
};
```

业务代码只需要知道有一个 `Logger`，不一定关心它究竟是控制台实现、文件实现还是网络实现。

## 这和“直接 new 一个具体类”有什么区别

区别主要体现在依赖方向上。

如果业务代码直接依赖 `ConsoleLogger`，那么以后想换成 `FileLogger` 或 `RemoteLogger`，调用方就更容易被牵连。  
如果业务代码只依赖 `Logger` 接口，那么替换实现时，变化通常会被限制在更小范围内。

这类设计最适合下面几种场景：

- 存在明确的替换点
- 未来可能会有多个实现
- 调用方只需要某种稳定能力

## 这其实是在控制依赖方向

接口编程最重要的收益之一，不只是“以后方便替换实现”，更是让高层模块不直接绑死在某个底层实现上。

也就是说：

- 业务层依赖 `Logger`
- 而不是直接依赖 `ConsoleLogger`

这样做之后，具体实现可以变化，但业务层对外表达的需求仍然稳定。

----

# 默认实现

接口并不一定意味着“每个函数都必须是纯虚函数”。

有些抽象基类会把一部分流程固定下来，把变化点留给派生类实现。这种写法有时被称为模板方法风格。

例如：

```cpp
#include <iostream>
#include <string>

class Logger {
public:
    virtual ~Logger() = default;

    void log(const std::string& text) {
        std::cout << "[begin] ";
        write(text);
        std::cout << "[end]" << std::endl;
    }

protected:
    virtual void write(const std::string& text) = 0;
};
```

这里：

- `log()` 提供了统一流程
- `write()` 留给派生类决定具体实现

## 什么时候值得这样做

当你发现：

- 整体流程是稳定的
- 只有局部步骤需要变化
- 多个实现共享同一套外层规则

这时抽象基类提供少量默认实现，往往比“所有东西都纯虚”更自然。

----

# 接口与组合

现代 `C++` 里，“接口”不一定意味着“深层继承树”。很多时候，更好的方式是：

- 定义窄接口
- 把能力拆小
- 通过组合拼接行为

例如一个类需要“可记录日志”和“可序列化”，未必需要从一个巨大的万能基类继承。

## 为什么窄接口更健康

接口一旦过大，常见问题会很快出现：

- 实现类被迫实现很多自己并不真正关心的方法
- 接口修改的影响面很大
- 调用方难以看出自己真正依赖了什么能力

所以现代设计更强调“小而稳定”的接口，而不是“一个基类包打天下”。

## 接口不是越多越好

很多人学到“依赖抽象”之后，会下意识到处造接口。  
但如果一个实现根本没有替换需求，或者系统里只有一个稳定实现，那么额外抽象反而可能只是增加代码层次。

所以更稳妥的判断是：

> **先有替换点和边界需求，再引入接口；不要为了形式统一而制造抽象。**

----

# `override`

接口类最怕实现类写错签名却没有真正覆盖成功：

```cpp
#include <string>

class Logger {
public:
    virtual ~Logger() = default;
    virtual void write(const std::string& text) = 0;
};

class FileLogger : public Logger {
public:
    void write(const std::string& text) override {
    }
};
```

只要是实现接口方法，就应该加 `override`。

## 它帮你挡掉什么错误

假设你本来想覆盖基类函数，却不小心写成了不同签名：

```cpp
void write(std::string text);
```

如果没有 `override`，这可能只是新增了一个同名函数，真正的覆盖根本没有发生。  
而加上 `override` 后，编译器会立刻指出问题。

所以 `override` 的价值不只是“代码更好看”，而是把接口实现错误尽早暴露在编译期。

----

# 智能指针

`C++11` 项目中，抽象接口对象常通过智能指针管理：

```cpp
#include <iostream>
#include <memory>
#include <string>

class Logger {
public:
    virtual ~Logger() = default;
    virtual void write(const std::string& text) = 0;
};

class ConsoleLogger : public Logger {
public:
    void write(const std::string& text) override {
        std::cout << text << std::endl;
    }
};

int main() {
    std::unique_ptr<Logger> logger = std::make_unique<ConsoleLogger>();
    logger->write("hello");
    return 0;
}
```

这能避免手动释放接口实现对象，也能把对象所有权写得更清楚。

## `unique_ptr<Interface>` 为什么常见

这种写法通常同时表达了两层信息：

- 对外暴露的是抽象能力边界
- 具体实现对象的生命周期由当前拥有者独占管理

这也是现代 `C++11` 中非常常见的依赖注入方式。你会经常看到一个类构造时接收 `std::unique_ptr<Interface>`，然后把实现对象保存为成员。

## 不只有 `unique_ptr` 这一种注入方式

接口依赖常见还有几种形式：

- `Interface&`：调用方不转移所有权，只是借用依赖
- `Interface*`：表达可为空或可选依赖时更常见
- `std::unique_ptr<Interface>`：表达独占所有权转移
- `std::shared_ptr<Interface>`：表达共享生命周期

所以关键不是“永远用哪一种”，而是：

> **参数形式要和所有权语义一致。**

----

# 组合优先

如果一个类只是“拥有某种能力”，而不是“本质上就是某种类型”，就更适合组合。

例如：

- `UserService` 拥有一个 `Logger`
- `OrderProcessor` 依赖一个 `Storage`

这类关系通常不该建成继承层次。

## 先问是不是稳定替换点

接口的价值在于解耦，但不是所有地方都值得抽象。

比较实用的判断方式是先问三个问题：

1. 这里是否真的存在稳定替换需求
2. 是否很可能出现多个实现
3. 调用方是否真的只关心能力，不关心具体类型

如果这些问题都不明显，那完全可以先不用接口。  
“依赖抽象”不等于“到处制造抽象”。

----

# 综合示例

```cpp
#include <iostream>
#include <memory>
#include <string>

class Storage {
public:
    virtual ~Storage() = default;
    virtual void save(const std::string& data) = 0;
};

class ConsoleStorage : public Storage {
public:
    void save(const std::string& data) override {
        std::cout << "save: " << data << std::endl;
    }
};

class UserService {
public:
    explicit UserService(std::unique_ptr<Storage> storage)
        : storage_(std::move(storage)) {
    }

    void createUser(const std::string& name) {
        storage_->save(name);
    }

private:
    std::unique_ptr<Storage> storage_;
};

int main() {
    UserService service(std::make_unique<ConsoleStorage>());
    service.createUser("alice");
    return 0;
}
```

这个例子体现了几个很重要的现代思路：

- 用抽象类定义能力边界
- 用 `override` 保证实现签名正确
- 用 `unique_ptr` 管理实现对象生命周期
- 用构造注入把依赖关系显式写进对象创建过程
- 用组合而不是深继承树组织业务依赖

----

# 推荐实践

- 接口类通常只保留必要的纯虚函数。
- 接口基类通常提供虚析构函数。
- 实现接口时始终加 `override`。
- 区分“纯接口类”和“带少量默认实现的抽象基类”。
- 优先设计窄接口，不要让一个基类承担过多职责。
- 只有存在真实替换需求时才抽象接口。
- 参数形式要和所有权语义匹配：借用、独占、共享要分清。
- 对外依赖抽象，对内组合具体实现。

----

# 小结

现代 `C++` 里的接口设计，重点不在于“找一个关键字替代别的语言的 `interface`”，而在于把能力边界、替换点、默认实现边界和对象生命周期设计清楚。

只要你能形成几个稳定判断标准: 什么时候该抽象、接口该做多窄、基类何时必须有虚析构、实现类为何总该写 `override`，接口设计这一章就算真正入门了。
