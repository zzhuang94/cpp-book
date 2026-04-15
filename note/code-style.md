# 编码规范

本文面向 `C++11` 及后续标准的工程代码。

目标不是追求风格炫技，而是建立一套便于协作、便于评审、便于重构的默认写法。

除非存在明确的性能、兼容性或历史原因，宜优先采用文中的推荐形式。

# 基本原则

## 可读优先

代码首先服务于维护者，其次才服务于编译器。

默认采用语义直接、边界清楚、便于静态检查的写法。

不推荐：

```cpp
int f(int a, int b, int c);
```

推荐：

```cpp
int calcRetryDelay(int retryCount, int baseDelayMs, int maxDelayMs);
```

不推荐：

```cpp
auto x = get();
```

推荐：

```cpp
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

不推荐：

```cpp
char* buffer = 0;
```

推荐：

```cpp
char* buffer = nullptr;
```

不推荐：

```cpp
std::FILE* fp = std::fopen(path, "r");
if (fp == nullptr) {
    return;
}
std::fclose(fp);
```

推荐：

```cpp
using FilePtr = std::unique_ptr<std::FILE, decltype(&std::fclose)>;

FilePtr fp(std::fopen(path, "r"), &std::fclose);
if (!fp) {
    return;
}
```

一句话原则如下。

> 正确用法应当自然发生，危险用法应当显得别扭。

# 基础写法

## 初始化

变量应在定义处初始化。

不推荐：

```cpp
int count;
count = 10;
```

推荐：

```cpp
int count{10};
```

不推荐：

```cpp
std::string name;
if (ok) {
    name = "alice";
}
```

推荐：

```cpp
std::string name = ok ? "alice" : "";
```

不推荐：

```cpp
std::vector<int> values;
values.push_back(1);
values.push_back(2);
values.push_back(3);
```

推荐：

```cpp
std::vector<int> values{1, 2, 3};
```

空指针统一使用 `nullptr`。

不推荐：

```cpp
Widget* widget = NULL;
```

推荐：

```cpp
Widget* widget = nullptr;
```

优先使用花括号初始化，但应理解其重载选择规则。

```cpp
int port{8080};
double ratio{0.5};
std::string userName{"alice"};
std::vector<int> ids{1, 2, 3};
```

当花括号可能触发 `initializer_list` 构造时，应先确认语义。

```cpp
std::vector<int> a(3, 10);
std::vector<int> b{3, 10};
```

上例中，`a` 与 `b` 的含义不同。

成员应在声明处或初始化列表中初始化。

不推荐：

```cpp
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
```

推荐：

```cpp
class Task {
public:
    Task() = default;

private:
    int retryCount_{3};
    int timeoutMs_{500};
};
```

初始化列表优先用于如下场景。

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

`const` 用于表达只读承诺，而不是装饰性关键字。

只读的大对象形参默认使用 `const T&`。

```cpp
void printUser(const std::string& userName);
void saveConfig(const Config& config);
```

成员函数若不修改对象可观察状态，应声明为 `const`。

```cpp
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
```

不推荐：

```cpp
class Counter {
public:
    int value() {
        return value_;
    }

private:
    int value_{0};
};
```

推荐：

```cpp
class Counter {
public:
    int value() const {
        return value_;
    }

private:
    int value_{0};
};
```

编译期常量优先使用 `constexpr`。

```cpp
constexpr int kMaxRetryCount{5};
constexpr std::size_t kBufferSize{4096};
constexpr auto kDefaultTimeout = std::chrono::seconds{30};
```

不推荐：

```cpp
#define MAX_RETRY 5
```

推荐：

```cpp
constexpr int kMaxRetry{5};
```

指针与 `const` 组合时，应写清限定位置。

```cpp
const Widget* reader = nullptr;
Widget* const owner = getOwner();
const Widget* const fixedReader = getReader();
```

若某个返回值不需要共享存储，应优先返回值对象，而不是返回 `const` 引用。

不推荐：

```cpp
const std::string& buildName();
```

推荐：

```cpp
std::string buildName();
```

## auto与推导

`auto` 的用途是减噪，不是隐藏语义。

适用场景如下。

- 右值类型明显。
- 模板类型冗长。
- 迭代器类型冗长。
- `lambda` 对象无法直接书写。

推荐：

```cpp
auto iter = userMap.find(userId);
auto worker = [](int value) { return value * 2; };
auto now = std::chrono::steady_clock::now();
```

不推荐：

```cpp
auto timeout = getConfig("timeout");
auto value = parse(input);
```

推荐：

```cpp
std::chrono::milliseconds timeout = getTimeoutConfig();
ParseResult result = parse(input);
```

若右值是工厂函数，且函数名已清楚表达类型或语义，可使用 `auto`。

```cpp
auto client = makeHttpClient();
auto token = buildAccessToken();
```

若数值精度重要，应显式写出类型。

不推荐：

```cpp
auto rate = 0.1;
```

推荐：

```cpp
double rate = 0.1;
```

结构化绑定虽可读，但不应滥用。

不推荐：

```cpp
auto [a, b, c] = getUserRecord();
```

推荐：

```cpp
UserRecord record = getUserRecord();
const auto& userName = record.name;
const auto& userEmail = record.email;
```

若结构体语义极清楚，结构化绑定可以接受。

```cpp
const auto [x, y] = point;
```

## 控制流程

控制流程应让主路径清楚可见。

宜优先过滤异常路径，再处理核心路径。

不推荐：

```cpp
void process(const Request& request) {
    if (request.isAuthorized()) {
        if (!request.items().empty()) {
            if (request.target() != nullptr) {
                execute(request);
            }
        }
    }
}
```

推荐：

```cpp
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

条件表达式应直接表达业务判定。

不推荐：

```cpp
if (status) {
    sync();
}
```

推荐：

```cpp
if (status == SyncStatus::ready) {
    sync();
}
```

不推荐在单个表达式中混合过多副作用。

不推荐：

```cpp
values[index++] = build(next++);
```

推荐：

```cpp
const int currentIndex = index;
++index;

const int currentValue = next;
++next;

values[currentIndex] = build(currentValue);
```

枚举宜优先使用 `enum class`。

```cpp
enum class TaskState {
    pending,
    running,
    finished,
    failed
};
```

`switch` 应尽量穷尽状态。

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

魔法数应提取为常量。

不推荐：

```cpp
retry(3, 5000);
```

推荐：

```cpp
constexpr int kRetryCount{3};
constexpr auto kRetryDelay = std::chrono::milliseconds{5000};

retry(kRetryCount, kRetryDelay);
```

三目表达式适合短小、无副作用、左右分支清晰的场景。

不推荐：

```cpp
auto result = isReady ? buildPrimaryResult(config, cache, logger)
                      : buildFallbackResult(config, cache, logger);
```

推荐：

```cpp
Result result;
if (isReady) {
    result = buildPrimaryResult(config, cache, logger);
} else {
    result = buildFallbackResult(config, cache, logger);
}
```

# 资源管理

## 所有权表达

所有权应写在类型里，而不是藏在注释里。

独占所有权默认使用值对象或 `std::unique_ptr`。

共享所有权仅在确有多个独立持有者时使用 `std::shared_ptr`。

不推荐：

```cpp
Widget* widget = new Widget();
use(widget);
delete widget;
```

推荐：

```cpp
auto widget = std::make_unique<Widget>();
use(widget.get());
```

若项目仍以 `C++11` 为基线，可写成如下形式。

```cpp
std::unique_ptr<Widget> widget(new Widget());
```

裸指针默认表示如下语义。

- 非拥有。
- 可为空。
- 受外部生命周期约束。

```cpp
void bindSession(Session* session);
```

引用默认表示如下语义。

- 非空。
- 非拥有。
- 生命周期由外部保证。

```cpp
void loadFromCache(Cache& cache);
void printUser(const User& user);
```

不推荐使用 `shared_ptr` 仅仅因为“传递方便”。

不推荐：

```cpp
void printUser(std::shared_ptr<User> user);
```

推荐：

```cpp
void printUser(const User& user);
```

如果需要观察共享对象但不参与拥有关系，可使用 `std::weak_ptr`。

```cpp
class Observer {
public:
    explicit Observer(std::weak_ptr<Model> model) : model_(std::move(model)) {
    }

    void refresh() {
        auto model = model_.lock();
        if (!model) {
            return;
        }

        draw(*model);
    }

private:
    std::weak_ptr<Model> model_;
};
```

循环引用应主动打断。

不推荐：

```cpp
class Parent {
public:
    std::shared_ptr<class Child> child;
};

class Child {
public:
    std::shared_ptr<Parent> parent;
};
```

推荐：

```cpp
class Parent {
public:
    std::shared_ptr<class Child> child;
};

class Child {
public:
    std::weak_ptr<Parent> parent;
};
```

资源包装对象宜遵循 `RAII`。

```cpp
class FileGuard {
public:
    explicit FileGuard(FILE* fp) noexcept : fp_(fp) {
    }

    ~FileGuard() noexcept {
        if (fp_ != nullptr) {
            std::fclose(fp_);
        }
    }

    FileGuard(const FileGuard&) = delete;
    FileGuard& operator=(const FileGuard&) = delete;

private:
    FILE* fp_{nullptr};
};
```

## 参数传递

参数传递形式应直接体现修改性、拥有关系与空值语义。

常见约定如下。

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

当函数需要把参数保存到成员中时，可按值接收。

```cpp
class Message {
public:
    explicit Message(std::string text) : text_(std::move(text)) {
    }

private:
    std::string text_;
};
```

这比同时维护左值与右值两个重载更简洁。

不推荐：

```cpp
class Message {
public:
    explicit Message(const std::string& text) : text_(text) {
    }

    explicit Message(std::string&& text) : text_(std::move(text)) {
    }

private:
    std::string text_;
};
```

若空值不合法，就不应用指针。

不推荐：

```cpp
void save(User* user);
```

推荐：

```cpp
void save(const User& user);
```

若函数必须修改外部对象，应使用非 `const` 引用。

```cpp
void sortScores(std::vector<int>& scores);
void reset(Session& session);
```

右值引用只在真正建模移动语义时使用，不要为了“现代写法”而滥用。

不推荐：

```cpp
void print(std::string&& text);
```

推荐：

```cpp
void print(const std::string& text);
```

对于连续内存，若可用 `std::span`，应优先使用。

```cpp
void parse(std::span<const std::byte> buffer);
```

若项目标准不足以使用 `std::span`，至少同时传递指针和长度。

```cpp
void parse(const char* data, std::size_t size);
```

参数数量过多通常意味着抽象层级不对。

不推荐：

```cpp
void connect(const std::string& host,
             int port,
             bool useTls,
             int timeoutMs,
             int retryCount,
             Logger* logger);
```

推荐：

```cpp
struct ConnectOptions {
    std::string host;
    int port{0};
    bool useTls{true};
    int timeoutMs{0};
    int retryCount{0};
};

void connect(const ConnectOptions& options, Logger* logger);
```

## 返回设计

按值返回通常是默认方案。

```cpp
std::string buildPath(const Config& config);
std::vector<User> loadUsers();
```

不推荐用输出参数堆叠结果。

不推荐：

```cpp
bool parse(const std::string& text, int& code, std::string& message);
```

推荐：

```cpp
struct ParseResult {
    int code{0};
    std::string message;
};

ParseResult parse(const std::string& text);
```

若结果可能不存在，可使用可空指针、状态值或 `std::optional`。

```cpp
std::optional<User> findUserById(int userId);
```

不推荐用空字符串、`-1` 或特殊值混合表达正常结果与失败。

不推荐：

```cpp
std::string queryUserName(int userId);
```

推荐：

```cpp
std::optional<std::string> queryUserName(int userId);
```

若返回的是借用视图，应保证生命周期足够长。

不推荐：

```cpp
const std::string& buildToken() {
    std::string token = "abc";
    return token;
}
```

推荐：

```cpp
std::string buildToken() {
    std::string token = "abc";
    return token;
}
```

返回指针时，应清楚说明是否允许为空以及谁负责释放。

不推荐：

```cpp
User* createUser();
```

推荐：

```cpp
std::unique_ptr<User> createUser();
```

若函数只是查询缓存中的对象，可返回观察指针。

```cpp
const User* findCachedUser(int userId) const;
```

# 命名规则

## 变量与函数

名称应表达业务语义，而不是局部书写方便。

不推荐：

```cpp
std::vector<int> v;
std::string s;
int n;
```

推荐：

```cpp
std::vector<int> scoreList;
std::string userName;
int retryCount;
```

布尔变量宜采用谓词形式。

```cpp
bool isReady{false};
bool hasPermission{true};
bool shouldRetry{false};
```

不推荐：

```cpp
bool flag{false};
bool status{true};
```

推荐：

```cpp
bool isCacheValid{false};
bool hasRemoteAccess{true};
```

函数名应体现动作或判定。

```cpp
void refreshCache();
bool isExpired() const;
std::string buildMessage() const;
```

不推荐：

```cpp
void process();
void handle();
bool check() const;
```

推荐：

```cpp
void processLoginRequest();
void handleDisconnect();
bool isSessionExpired() const;
```

计量值应尽量带单位。

```cpp
int timeoutMs{500};
int retryCount{3};
std::size_t bufferSize{4096};
```

缩写应克制使用。

可接受：

```cpp
std::string userId;
std::string requestUrl;
HttpClient httpClient;
```

不推荐：

```cpp
std::string uid;
std::string reqUrl;
HttpClient hc;
```

局部索引变量可以简短，但作用域一旦扩大就应完整命名。

```cpp
for (std::size_t i = 0; i < values.size(); ++i) {
    std::cout << values[i] << '\n';
}
```

```cpp
std::size_t failedTaskIndex = 0;
```

## 类型与常量

类型名宜采用名词或名词短语。

```cpp
class UserRepository;
class HttpClient;
struct ParseResult;
enum class LogLevel;
```

不推荐：

```cpp
class DoUser;
class DataHandle;
```

推荐：

```cpp
class UserService;
class DataParser;
```

常量应具备业务名称，而非仅有数值。

```cpp
constexpr int kMaxConnections{64};
constexpr int kDefaultRetryCount{3};
constexpr auto kHeartbeatInterval = std::chrono::seconds{5};
```

枚举应使用 `enum class`，枚举项不必重复类型前缀。

不推荐：

```cpp
enum LogLevel {
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
};
```

推荐：

```cpp
enum class LogLevel {
    info,
    warn,
    error
};
```

命名空间应简短、稳定、与业务边界一致。

```cpp
namespace net {
class Client;
}

namespace storage {
class Repository;
}
```

宏名称若不可避免，应足够独特并全部大写。

```cpp
#define CPP_BOOK_ENABLE_TRACE 1
```

# 类型设计

## 类与职责

类应围绕稳定职责构建。

不推荐把多个层次的责任塞进同一个类。

不推荐：

```cpp
class UserManager {
public:
    void loadUsers();
    void validatePassword();
    void sendEmail();
    void writeAuditLog();
    void connectDatabase();
};
```

推荐：

```cpp
class UserRepository {
public:
    std::vector<User> loadUsers() const;
};

class PasswordValidator {
public:
    bool validate(const std::string& password) const;
};

class AuditLogger {
public:
    void write(const AuditEvent& event);
};
```

构造函数应让对象一经创建即处于有效状态。

不推荐：

```cpp
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
```

推荐：

```cpp
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

若某类具有独占资源，应明确复制与移动策略。

```cpp
class Socket {
public:
    Socket() = delete;
    explicit Socket(int fd) noexcept : fd_(fd) {
    }

    ~Socket() noexcept;

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

private:
    int fd_{-1};
};
```

成员访问级别应服务于不变量维护。

不推荐：

```cpp
struct UserProfile {
    std::string name;
    int age;
    bool deleted;
};
```

推荐：

```cpp
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

## 函数设计

函数应只做一件逻辑上完整的事。

若一个函数同时包含校验、计算、持久化、日志与回滚，则通常过大。

不推荐：

```cpp
void createOrder(const Request& request) {
    validate(request);
    Order order = buildOrder(request);
    saveOrder(order);
    writeAuditLog(order);
    notifyUser(order);
}
```

推荐：

```cpp
Order createOrder(const Request& request) {
    validateRequest(request);
    Order order = buildOrder(request);
    persistOrder(order);
    return order;
}

void afterOrderCreated(const Order& order) {
    writeAuditLog(order);
    notifyUser(order);
}
```

复杂条件可拆为具名判定函数。

不推荐：

```cpp
if (request.user() != nullptr &&
    request.user()->isActive() &&
    request.items().size() < kMaxItems &&
    !request.token().empty()) {
    submit(request);
}
```

推荐：

```cpp
bool canSubmit(const Request& request) {
    return request.user() != nullptr &&
           request.user()->isActive() &&
           request.items().size() < kMaxItems &&
           !request.token().empty();
}

if (canSubmit(request)) {
    submit(request);
}
```

函数名应能概括行为。

不推荐：

```cpp
void run();
```

推荐：

```cpp
void runBackgroundSync();
```

输出参数应克制使用。

不推荐：

```cpp
void splitName(const std::string& fullName,
               std::string& firstName,
               std::string& lastName);
```

推荐：

```cpp
struct NameParts {
    std::string firstName;
    std::string lastName;
};

NameParts splitName(const std::string& fullName);
```

查询函数不应隐含重型副作用。

不推荐：

```cpp
const User& currentUser();
```

若该函数内部可能触发网络请求、缓存刷新或磁盘加载，则名称不足以表达其成本与风险。

推荐：

```cpp
User loadCurrentUser();
```

## 继承与多态

继承只应用于稳定的“是一个”关系。

若只是复用代码，通常优先组合。

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

不推荐：

```cpp
class ConsolePrinter : public Printer {
public:
    void print(const std::string& text) {
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

头文件应自包含。

一个头文件被单独包含时，应能完成其声明所需的前置依赖。

包含顺序宜统一。

- 当前文件对应头文件。
- 标准库头文件。
- 第三方头文件。
- 项目内头文件。

示例：

```cpp
#include "service.h"

#include <memory>
#include <string>

#include "third_party/json.hpp"

#include "model/user.h"
#include "storage/repository.h"
```

只需要声明时，应优先前向声明。

```cpp
class Logger;

class Service {
public:
    explicit Service(Logger& logger);

private:
    Logger& logger_;
};
```

若成员是对象实体而非指针或引用，则必须包含完整定义。

```cpp
#include <string>

class User {
private:
    std::string name_;
};
```

头文件中不得使用 `using namespace`。

不推荐：

```cpp
using namespace std;
```

推荐：

```cpp
std::string name;
std::vector<int> values;
```

包含保护应统一使用一种机制。

```cpp
#pragma once
```

或：

```cpp
#ifndef CPP_BOOK_USER_H
#define CPP_BOOK_USER_H

class User {};

#endif
```

## 宏与接口

宏应尽量减少。

能用常量、内联函数、模板替代的地方，不应用宏。

不推荐：

```cpp
#define SQUARE(x) ((x) * (x))
```

推荐：

```cpp
template <typename T>
inline T square(const T& value) {
    return value * value;
}
```

不推荐：

```cpp
#define BUFFER_SIZE 4096
```

推荐：

```cpp
constexpr std::size_t kBufferSize{4096};
```

条件编译应集中在平台适配层，不宜散落在业务逻辑中。

不推荐：

```cpp
void writeLog(const std::string& text) {
#ifdef _WIN32
    writeWindowsLog(text);
#else
    writePosixLog(text);
#endif
}
```

推荐：

```cpp
class PlatformLogger {
public:
    void write(const std::string& text);
};

void writeLog(PlatformLogger& logger, const std::string& text) {
    logger.write(text);
}
```

头文件暴露的是模块契约。

凡是出现在公共头文件中的类型、宏、内联实现与模板细节，都应视为潜在的长期承诺。

# 工程实践

## 注释与文档

注释用于解释代码表面看不出的信息。

适合写入注释的内容如下。

- 设计意图。
- 时序约束。
- 协议背景。
- 性能取舍。
- 历史兼容原因。

不推荐：

```cpp
// i 加 1
++i;
```

推荐：

```cpp
// 必须先更新本地状态，再通知观察者，否则回调会读到旧值
updateState();
notifyObservers();
```

不推荐：

```cpp
// TODO
```

推荐：

```cpp
// TODO: 第三方接口升级后改为批量查询，当前循环仅作为兼容方案
```

公共接口若存在非显然约束，应辅以简短文档。

```cpp
// 调用前要求 session 已完成认证。
// 失败时返回空结果，不抛出异常。
std::optional<User> loadCurrentUser(const Session& session);
```

文档示例应尽量可运行，而不是只描述步骤。

```cpp
Config config;
config.host = "127.0.0.1";
config.port = 8080;

Client client(config);
client.connect();
```

## 错误处理

错误处理策略应在项目内保持一致。

若使用异常，则异常只用于异常路径，不应用于普通流程分支。

若使用返回值表达失败，则应统一返回类型与诊断信息形式。

不推荐：

```cpp
bool save(const User& user);
int load();
std::string parse();
```

上例中，不同函数使用不同失败约定，调用方难以形成稳定心智模型。

推荐统一风格，例如：

```cpp
struct Status {
    bool ok{false};
    std::string message;
};

Status save(const User& user);
Status load();
Status parse();
```

或：

```cpp
void save(const User& user);
User load();
ParseResult parse();
```

并在项目文档中明确哪些接口会抛出异常。

析构函数通常不应抛出异常。

```cpp
class Writer {
public:
    ~Writer() noexcept {
        tryClose();
    }

private:
    void tryClose() noexcept;
};
```

断言用于内部不变量，不用于外部输入校验。

不推荐：

```cpp
void parsePort(int port) {
    assert(port > 0 && port < 65536);
}
```

若 `port` 来自外部输入，上述做法不足以替代正常校验。

推荐：

```cpp
bool isValidPort(int port) {
    return port > 0 && port < 65536;
}
```

错误日志应包含足够上下文。

不推荐：

```cpp
logger.error("save failed");
```

推荐：

```cpp
logger.error("save failed, userId={}, path={}", user.id(), filePath);
```

## 评审与测试

代码评审应优先关注高风险语义问题。

优先级较高的检查项如下。

- 所有权是否清楚。
- 生命周期是否安全。
- 参数与返回值是否合理。
- 错误处理是否一致。
- 命名是否与业务一致。

格式类问题宜交由工具处理。

测试应覆盖最容易出错的边界。

```cpp
TEST(ParsePortTest, RejectsNegativeValue) {
    EXPECT_FALSE(isValidPort(-1));
}

TEST(ParsePortTest, RejectsZero) {
    EXPECT_FALSE(isValidPort(0));
}

TEST(ParsePortTest, AcceptsNormalPort) {
    EXPECT_TRUE(isValidPort(8080));
}
```

资源管理类对象宜测试复制、移动与析构行为。

```cpp
TEST(SocketTest, IsNotCopyConstructible) {
    EXPECT_FALSE(std::is_copy_constructible<Socket>::value);
}

TEST(SocketTest, IsMoveConstructible) {
    EXPECT_TRUE(std::is_move_constructible<Socket>::value);
}
```

解析类逻辑宜覆盖正常输入、边界输入与非法输入。

```cpp
TEST(ParserTest, ParsesEmptyList) {
    ParseResult result = parse("[]");
    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.items.empty());
}

TEST(ParserTest, RejectsBrokenJson) {
    ParseResult result = parse("[");
    EXPECT_FALSE(result.ok);
}
```

测试命名应直接反映行为。

不推荐：

```cpp
TEST(Test1, Case1) {
}
```

推荐：

```cpp
TEST(UserServiceTest, ReturnsNulloptWhenUserIsInactive) {
}
```

# 综合示例

## 服务示例

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

## 自检清单

提交前可快速核对下列问题。

- 是否所有变量都在定义处初始化。
- 是否统一使用 `nullptr`。
- `auto` 是否只用于减噪场景。
- 只读参数是否优先使用 `const T&`。
- 可空关系是否明确使用指针或可选类型。
- 裸 `new`/`delete` 是否已经消除。
- 共享所有权是否确有必要。
- 返回值是否真实表达失败与缺失。
- 布尔名称是否采用谓词形式。
- 继承是否真的比组合更合适。
- 虚函数覆盖是否标注 `override`。
- 头文件是否避免 `using namespace`。
- 宏是否已压缩到必要范围。
- 注释是否解释了代码看不出的约束。
- 错误处理是否与模块保持一致。
- 关键边界是否已有测试覆盖。

若以上多数问题都能得到正面回答，则代码通常已具备较好的维护基础。
