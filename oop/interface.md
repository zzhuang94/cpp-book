# 接口

`C++` 没有像某些语言那样单独的 `interface` 关键字，但这并不意味着它没有接口设计能力。相反，`C++` 常通过抽象基类、纯虚函数和组合方式来表达接口。到了 `C++11`，`override`、`final` 和智能指针让接口设计变得更安全、更现代。

## 纯虚函数与抽象类

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

## 为什么接口类通常要有虚析构函数

接口常通过基类指针或引用使用：

```cpp
Drawable* p = new Button();
delete p;
```

如果析构函数不是虚的，就可能析构不完整，造成资源泄漏或未定义行为。

## 面向接口编程

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

## 接口与组合

现代 `C++` 里，“接口”不一定意味着“深层继承树”。很多时候，更好的方式是：

- 定义窄接口
- 把能力拆小
- 通过组合拼接行为

例如一个类需要“可记录日志”和“可序列化”，未必需要从一个巨大的万能基类继承。

## `override` 的价值

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

## 使用智能指针管理接口对象

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
    std::unique_ptr<Logger> logger(new ConsoleLogger());
    logger->write("hello");
    return 0;
}
```

这能避免手动释放接口实现对象。

## 什么时候优先组合而不是继承

如果一个类只是“拥有某种能力”，而不是“本质上就是某种类型”，就更适合组合。

例如：

- `UserService` 拥有一个 `Logger`
- `OrderProcessor` 依赖一个 `Storage`

这类关系通常不该建成继承层次。

## 一个综合示例

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

## 推荐实践

- 接口类通常只保留必要的纯虚函数。
- 接口基类通常提供虚析构函数。
- 实现接口时始终加 `override`。
- 对外依赖抽象，对内组合具体实现。
- 只有确实存在稳定抽象边界时才设计接口，不要为“看起来高级”而泛化。

## 小结

在现代 `C++` 中，接口设计的关键不在于“模仿别的语言的 interface 关键字”，而在于把能力边界、扩展点和资源管理关系设计清楚。`C++11` 的语言特性和标准库工具，让这件事比旧时代更容易做对。
