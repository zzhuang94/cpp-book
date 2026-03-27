# 接口

`C++` 没有像某些语言那样单独的 `interface` 关键字，但这并不意味着它没有接口设计能力。相反，`C++` 常通过抽象基类、纯虚函数和组合方式来表达接口。到了 `C++11`，`override`、`final` 和智能指针让接口设计变得更安全、更现代。

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

带有纯虚函数的类不能直接实例化。

# 虚析构函数

接口常通过基类指针或引用使用：

```cpp
Drawable* p = new Button();
delete p;
```

如果析构函数不是虚的，就可能析构不完整，造成资源泄漏或未定义行为。

# 接口编程

接口的核心价值在于“调用方依赖能力，而不是依赖具体实现”。

```cpp
#include <iostream>

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

业务代码只需要知道有一个 `Logger`，不一定关心它具体是控制台实现、文件实现还是网络实现。

# 接口与组合

现代 `C++` 里，“接口”不一定意味着“深层继承树”。很多时候，更好的方式是：

- 定义窄接口
- 把能力拆小
- 通过组合拼接行为

例如一个类需要“可记录日志”和“可序列化”，未必需要从一个巨大的万能基类继承。

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

# 智能指针

`C++11` 项目中，抽象接口对象常通过智能指针管理：

```cpp
#include <iostream>
#include <memory>
#include <string>
#include <utility>

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
    std::unique_ptr<Logger> logger(new ConsoleLogger());
    logger->write("hello");
    return 0;
}
```

这能避免手动释放接口实现对象。

# 组合优先

如果一个类只是“拥有某种能力”，而不是“本质上就是某种类型”，就更适合组合。

例如：

- `UserService` 拥有一个 `Logger`
- `OrderProcessor` 依赖一个 `Storage`

这类关系通常不该建成继承层次。

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
    UserService service(std::unique_ptr<Storage>(new ConsoleStorage()));
    service.createUser("alice");
    return 0;
}
```

这个例子体现了两个现代思路：

- 用抽象类定义能力边界
- 用组合注入具体实现

# 推荐实践

- 接口类通常只保留必要的纯虚函数。
- 接口基类通常提供虚析构函数。
- 实现接口时始终加 `override`。
- 对外依赖抽象，对内组合具体实现。
- 只有确实存在稳定抽象边界时才设计接口，不要为“看起来高级”而泛化。

# 小结

在现代 `C++` 中，接口设计的关键不在于“模仿别的语言的 interface 关键字”，而在于把能力边界、扩展点和资源管理关系设计清楚。`C++11` 的语言特性和标准库工具，让这件事比旧时代更容易做对。

## 对其它语言开发者来说，这章最需要转变的是什么

如果你来自有显式 `interface` 关键字的语言，刚看 `C++` 时可能会觉得接口设计不够直接。但更准确的理解是：

- `C++` 把接口表达能力分散到了抽象类、虚函数、组合和模板里
- 这使它更灵活，但也更要求你主动做设计判断

所以这章的重点不是找一个语法糖替代 `interface`，而是学会“如何把能力边界设计清楚”。

## 窄接口通常比大接口更健康

一个常见的设计问题是基类过大，什么都想抽象，最后导致：

- 实现类被迫实现很多无关方法
- 接口修改影响面巨大
- 调用方难以理解真正依赖了什么能力

现代设计更推荐把接口做窄，让每个接口只表达一小组稳定能力。

## 依赖抽象不等于到处造抽象

接口的价值在于解耦，但不是所有类都需要抽象基类。更合理的判断通常是：

- 这里是否真的存在稳定替换点
- 是否真的会有多个实现
- 调用方是否真的只需要“能力”，不需要具体类型

如果答案都不明显，那完全可以先不用接口。

## `unique_ptr<Interface>` 是很常见的组合方式

这类写法有两个关键信息：

- 通过抽象类型暴露能力边界
- 通过智能指针明确实现对象的生命周期管理

这也是现代 `C++11` 工程里非常常见的依赖注入方式。

## 常见误区

### 误区 1：没有 `interface` 关键字说明接口能力弱

不对。`C++` 只是采用了不同表达方式。

### 误区 2：只要想设计得高级一点，就该先抽接口

不对。没有真实替换需求时，过早抽象反而增加复杂度。

### 误区 3：接口越大越通用越好

不对。大接口通常更脆弱、更难维护。

## 补充建议

- 先设计小而稳定的能力边界。
- 只有存在真实替换需求时再抽象接口。
- 接口基类通常应提供虚析构函数。
- 实现类覆盖接口方法时始终加 `override`。
- 优先通过组合而不是深继承树来组织依赖。
