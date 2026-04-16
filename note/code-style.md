# 编码规范

本文面向 `C++11` 及后续标准的工程代码。  
目标不是追求风格炫技，而是建立一套便于协作、便于评审、便于重构的默认写法。  
除非存在明确的性能、兼容性或历史原因，宜优先采用文中的推荐形式。

----

# 基本原则

## 可读优先

代码首先服务于维护者，其次才服务于编译器。  
默认采用语义直接、边界清楚、便于静态检查的写法。

```cpp
// 不推荐：
int f(int a, int b, int c);
// 推荐：
int calcRetryDelay(int retryCount, int baseDelayMs, int maxDelayMs);

//不推荐：
auto x = get();
// 推荐：
ConnectionOptions options = getConnectionOptions();
```

规则要点如下。

- 名称应表达业务含义。
- 签名应表达所有权与修改语义。
- 控制流程应让主路径清楚可见。
- 复杂约束应交由类型系统表达。

## 默认安全

默认写法应尽量降低如下风险。

- 未初始化。
- 空指针误用。
- 资源泄漏。
- 生命周期悬垂。
- 隐式转换导致的误判。

```cpp
// 不推荐：
char* buffer = 0;
// 推荐：
char* buffer = nullptr;

// 不推荐：
std::FILE* fp = std::fopen(path, "r");
if (fp == nullptr) {
    return;
}
std::fclose(fp);
// 推荐：
using FilePtr = std::unique_ptr<std::FILE, decltype(&std::fclose)>;
FilePtr fp(std::fopen(path, "r"), &std::fclose);
if (!fp) {
    return;
}
```

> 一句话原则：**正确用法应当自然发生，危险用法应当显得别扭。**

----

# 基础写法

## 初始化

> 变量应在定义处初始化。

```cpp
// 不推荐：
int count;
count = 10;
// 推荐：
int count{10};

// 不推荐：
std::string name;
if (ok) {
    name = "alice";
}
// 推荐：
std::string name = ok ? "alice" : "";

//不推荐：
std::vector<int> values;
values.push_back(1);
values.push_back(2);
values.push_back(3);
// 推荐：
std::vector<int> values{1, 2, 3};
```

> 空指针统一使用 `nullptr`。

```cpp
// 不推荐：
Widget* widget = NULL;
// 推荐：
Widget* widget = nullptr;
```

> 优先使用花括号初始化，但应理解其重载选择规则。

```cpp
int port{8080};
double ratio{0.5};
std::string userName{"alice"};
std::vector<int> ids{1, 2, 3};

// 当花括号可能触发 `initializer_list` 构造时，应先确认语义。
std::vector<int> a(3, 10);  // [10, 10, 10]
std::vector<int> b{3, 10};  // [3, 10]
```

> 成员应在声明处或初始化列表中初始化。

```cpp
// 不推荐：
class Task {
public:
    Task() {
        retryCount_ = 3;
        timeoutMs_ = 500;
    }
private:
    int retryCount_;
    int timeoutMs_;
};
// 推荐：
class Task {
public:
    Task() = default;
private:
    int retryCount_{3};
    int timeoutMs_{500};
};
```

初始化列表优先用于如下场景：  
- `const` 成员。
- 引用成员。
- 无默认构造函数的成员。
- 需要移动构造的成员。

```cpp
class Session {
public:
    Session(std::string token, int userId)
        : token_(std::move(token)), userId_(userId) {
    }
private:
    std::string token_;
    int userId_{0};
};
```

## const与常量

> `const` 用于表达只读承诺，而不是装饰性关键字。

```cpp
// 只读的大对象形参默认使用 `const T&`。
void printUser(const std::string& userName);
void saveConfig(const Config& config);

// 成员函数若不修改对象可观察状态，应声明为 `const`。
class User {
public:
    const std::string& name() const {
        return name_;
    }
    bool isActive() const {
        return active_;
    }
private:
    std::string name_;
    bool active_{false};
};

// 不推荐：
class Counter {
public:
    int value() {
        return value_;
    }
private:
    int value_{0};
};
// 推荐：
class Counter {
public:
    int value() const {
        return value_;
    }
private:
    int value_{0};
};
```

> 编译期常量优先使用 `constexpr`。

```cpp
constexpr int kMaxRetryCount{5};
constexpr std::size_t kBufferSize{4096};
constexpr auto kDefaultTimeout = std::chrono::seconds{30};

// 不推荐：
#define MAX_RETRY 5
// 推荐：
constexpr int kMaxRetry{5};
```

> 指针与 `const` 组合时，应写清限定位置。

```cpp
const Widget* reader = nullptr;
Widget* const owner = getOwner();
const Widget* const fixedReader = getReader();
```

> 若某个返回值不需要共享存储，应优先返回值对象，而不是返回 `const` 引用。

```cpp
// 不推荐：
const std::string& buildName();

std::string buildName();
```

## auto与推导

> `auto` 的用途是减噪，不是隐藏语义。

适用场景如下：  
- 右值类型明显。
- 模板类型冗长。
- 迭代器类型冗长。
- `lambda` 对象无法直接书写。

```cpp
// 推荐：
auto iter = userMap.find(userId);
auto worker = [](int value) { return value * 2; };
auto now = std::chrono::steady_clock::now();

// 不推荐：
auto timeout = getConfig("timeout");
auto value = parse(input);
// 推荐：
std::chrono::milliseconds timeout = getTimeoutConfig();
ParseResult result = parse(input);
```

> 若右值是工厂函数，且函数名已清楚表达类型或语义，可使用 `auto`。

```cpp
auto client = makeHttpClient();
auto token = buildAccessToken();
```

> 若数值精度重要，应显式写出类型。

```cpp
// 不推荐：
auto rate = 0.1;
// 推荐：
double rate = 0.1;
```

> 结构化绑定虽可读，但不应滥用。

```cpp
// 不推荐：
auto [a, b, c] = getUserRecord();
// 推荐：
UserRecord record = getUserRecord();
const auto& userName = record.name;
const auto& userEmail = record.email;

// 若结构体语义极清楚，结构化绑定可以接受。
const auto [x, y] = point;
```

## 控制流程

> 控制流程应让主路径清楚可见，宜优先过滤异常路径，再处理核心路径。

```cpp
// 不推荐：
void process(const Request& request) {
    if (request.isAuthorized()) {
        if (!request.items().empty()) {
            if (request.target() != nullptr) {
                execute(request);
            }
        }
    }
}

// 推荐：
void process(const Request& request) {
    if (!request.isAuthorized()) {
        return;
    }
    if (request.items().empty()) {
        return;
    }
    if (request.target() == nullptr) {
        return;
    }
    execute(request);
}
```

> 条件表达式应直接表达业务判定。

```cpp
// 不推荐：
if (status) {
    sync();
}
// 推荐：
if (status == SyncStatus::ready) {
    sync();
}
```

> 枚举宜优先使用 `enum class`。

```cpp
enum class TaskState {
    pending,
    running,
    finished,
    failed
};
```

> `switch` 应尽量穷尽状态。

```cpp
switch (state) {
case TaskState::pending:
    prepare();
    break;
case TaskState::running:
    execute();
    break;
case TaskState::finished:
    finish();
    break;
case TaskState::failed:
    rollback();
    break;
}
```

> 魔法数应提取为常量。

```cpp
// 不推荐：
retry(3, 5000);

// 推荐：
constexpr int kRetryCount{3};
constexpr auto kRetryDelay = std::chrono::milliseconds{5000};

retry(kRetryCount, kRetryDelay);
```

----

# 资源管理

## 所有权表达

- 所有权应写在类型里，而不是藏在注释里。
- 独占所有权默认使用值对象或 `std::unique_ptr`。
- 共享所有权仅在确有多个独立持有者时使用 `std::shared_ptr`。

```cpp
// 不推荐：
Widget* widget = new Widget();
use(widget);
delete widget;

// 推荐：
auto widget = std::make_unique<Widget>();
use(widget.get());
// C++11 要写成如下形式。
std::unique_ptr<Widget> widget(new Widget());
```

- **裸指针** 默认语义：
  - 非拥有。
  - 可为空。
  - 受外部生命周期约束。
- **引用** 默认语义：
  - 非空。
  - 非拥有。
  - 生命周期由外部保证。

## 参数传递

参数传递形式应直接体现修改性、拥有关系与空值语义，常见约定如下：
- 小型标量按值传递。
- 只读大对象使用 `const T&`。
- 需要修改调用方对象时使用 `T&`。
- 可为空的非拥有对象使用 `T*`。
- 需要内部持有副本时，可按值接收再移动。

```cpp
void setAge(int age);
void printName(const std::string& name);
void normalize(std::vector<int>& values);
void attachLogger(Logger* logger);
```

> 当函数需要把参数保存到成员中时，可按值接收。  
> 这比同时维护左值与右值两个重载更简洁。

```cpp
// 推荐：
class Message {
public:
    explicit Message(std::string text) : text_(std::move(text)) {}
private:
    std::string text_;
};

// 不推荐：
class Message {
public:
    explicit Message(const std::string& text) : text_(text) {}
    explicit Message(std::string&& text) : text_(std::move(text)) {}
private:
    std::string text_;
};
```

> 若空值不合法，就不应用指针。

```cpp
// 不推荐：
void save(User* user);

// 推荐：
void save(const User& user);
```

> 若函数必须修改外部对象，应使用非 `const` 引用。

```cpp
void sortScores(std::vector<int>& scores);
void reset(Session& session);
```

!> 右值引用只在真正建模移动语义时使用，*不要为了“现代写法”而滥用。*

```cpp
// 不推荐：
void print(std::string&& text);
// 推荐：
void print(const std::string& text);
```

----

# 面向对象

## 类定义

> 构造函数应让对象一经创建即处于有效状态。

```cpp
// 不推荐：
class Client {
public:
    Client() = default;

    void init(const std::string& host, int port) {
        host_ = host;
        port_ = port;
    }
private:
    std::string host_;
    int port_{0};
};

// 推荐：
class Client {
public:
    Client(std::string host, int port)
        : host_(std::move(host)), port_(port) {
    }
private:
    std::string host_;
    int port_{0};
};
```

> 若某类具有独占资源，应明确复制与移动策略。

```cpp
class Socket {
public:
    ~Socket() noexcept;
    explicit Socket(int fd) noexcept : fd_(fd) {}

    Socket() = delete;                          // 禁止了​​默认构造函数​​（无参构造）
    Socket(const Socket&) = delete;             // 禁止拷贝构造函数
    Socket& operator=(const Socket&) = delete;  // 禁止拷贝赋值运算符​​

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

private:
    int fd_{-1};
};
```

> 成员访问级别应服务于不变量维护。

```cpp
// 不推荐：
struct UserProfile {
    std::string name;
    int age;
    bool deleted;
};

// 推荐：
class UserProfile {
public:
    explicit UserProfile(std::string name, int age)
        : name_(std::move(name)), age_(age) {
    }
    void markDeleted() {
        deleted_ = true;
    }
    bool isDeleted() const {
        return deleted_;
    }
private:
    std::string name_;
    int age_{0};
    bool deleted_{false};
};
```

## 继承与多态

继承只应用于稳定的“是一个”关系，若只是复用代码，通常优先组合。

多态基类应具有虚析构函数。

```cpp
class Printer {
public:
    virtual ~Printer() = default;
    virtual void print(const std::string& text) = 0;
};
```

覆盖虚函数时应始终标注 `override`。

```cpp
class ConsolePrinter : public Printer {
public:
    void print(const std::string& text) override {
        std::cout << text << '\n';
    }
};
```

若不允许继续派生，可使用 `final`。

```cpp
class JsonParser final : public Parser {
public:
    ParseResult parse(const std::string& text) override;
};
```

深层继承应避免。

不推荐：

```cpp
class A {};
class B : public A {};
class C : public B {};
class D : public C {};
```

推荐使用组合表达能力叠加。

```cpp
class RetryPolicy {
public:
    bool shouldRetry(int count) const;
};

class HttpClient {
public:
    explicit HttpClient(RetryPolicy policy) : policy_(std::move(policy)) {
    }

private:
    RetryPolicy policy_;
};
```

# 头文件规范

## 包含与声明

- 头文件应 **自包含**。
- 一个头文件被单独包含时，应能完成其声明所需的前置依赖。
- 包含顺序宜统一。
  - 当前文件对应头文件。
  - 标准库头文件。
  - 第三方头文件。
  - 项目内头文件。

```cpp
#include "service.h"

#include <memory>
#include <string>

#include "third_party/json.hpp"

#include "model/user.h"
#include "storage/repository.h"
```

> 只需要声明时，应优先前向声明。

```cpp
class Logger;

class Service {
public:
    explicit Service(Logger& logger);
private:
    Logger& logger_;
};
```

> 若成员是对象实体而非指针或引用，则必须包含完整定义。

```cpp
#include <string>

class User {
private:
    std::string name_;
};
```

## 宏与接口

> **宏应尽量减少**，能用常量、内联函数、模板替代的地方，不应用宏。

```cpp
// 不推荐：
#define SQUARE(x) ((x) * (x))
// 推荐：
template <typename T>
inline T square(const T& value) {
    return value * value;
}

// 不推荐：
#define BUFFER_SIZE 4096
// 推荐：
constexpr std::size_t kBufferSize{4096};
```

> 条件编译应集中在平台适配层，不宜散落在业务逻辑中。

```cpp
// 不推荐：
void writeLog(const std::string& text) {
#ifdef _WIN32
    writeWindowsLog(text);
#else
    writePosixLog(text);
#endif
}

// 推荐：
class PlatformLogger {
public:
    void write(const std::string& text);
};
void writeLog(PlatformLogger& logger, const std::string& text) {
    logger.write(text);
}
```

> 头文件暴露的是模块契约。  
> 凡是出现在公共头文件中的类型、宏、内联实现与模板细节，都应视为潜在的长期承诺。

----

# 综合示例

下例综合体现如下规则。

- 构造注入依赖。
- 明确共享与只读语义。
- 使用 `override`。
- 使用 `std::optional` 表达可缺失结果。
- 使用早返回表达失败路径。

```cpp
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <utility>

struct User {
    int id{0};
    std::string name;
    bool active{false};
};

class UserStore {
public:
    virtual ~UserStore() = default;
    virtual std::optional<User> findById(int userId) const = 0;
};

class ConsoleLogger {
public:
    void info(const std::string& text) const {
        std::cout << "[info] " << text << '\n';
    }

    void warn(const std::string& text) const {
        std::cout << "[warn] " << text << '\n';
    }
};

class UserService {
public:
    UserService(std::shared_ptr<const UserStore> store, ConsoleLogger logger)
        : store_(std::move(store)), logger_(std::move(logger)) {
    }

    std::optional<std::string> queryActiveUserName(int userId) const {
        if (userId <= 0) {
            logger_.warn("invalid user id");
            return std::nullopt;
        }

        const auto user = store_->findById(userId);
        if (!user.has_value()) {
            logger_.warn("user not found");
            return std::nullopt;
        }

        if (!user->active) {
            logger_.info("user inactive");
            return std::nullopt;
        }

        return user->name;
    }

private:
    std::shared_ptr<const UserStore> store_;
    ConsoleLogger logger_;
};
```

同一需求的反例如下。

```cpp
class BadService {
public:
    std::string query(int id, bool flag, int mode, UserStore* store);
};
```

该反例存在以下问题。

- 函数名语义过弱。
- 参数含义不透明。
- 指针所有权不清。
- 返回值无法区分失败与空结果。
