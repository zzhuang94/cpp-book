# CMake 实战

> 如果说“构建工具”那篇是在回答“`cmake` 是什么”，那么这一篇更关注另一个问题：  
> **拿到一个真实项目后，`CMake` 到底该怎么写、怎么拆、怎么维护。**

很多初学者在学完最小 `CMakeLists.txt` 之后，会马上遇到新的困惑：

- 一个项目有很多目录，`CMakeLists.txt` 该怎么拆
- 可执行程序、静态库、动态库要怎么组织
- 第三方库怎么接进来
- 测试怎么接入
- 安装和发布要怎么做
- 为什么有些 `CMake` 项目能读懂，有些像“魔法脚本”

这篇文档默认你已经知道最基础的几条命令：

- `project()`
- `add_executable()`
- `add_library()`
- `target_link_libraries()`

接下来我们把视角从“一个小例子能跑”升级到“一个工程如何长期维护”。

----

# 先建立一个实战视角

## 一个真实项目里，CMake 通常在做什么

在现代 `C++` 项目中，`CMake` 往往承担这些职责：

- 定义有哪些构建目标
- 描述目标之间的依赖关系
- 设置编译标准和编译选项
- 组织多目录项目
- 管理第三方依赖
- 集成测试
- 支持安装、打包、导出
- 支持不同平台和不同编译器

所以你写 `CMake` 时，不是在“拼命令字符串”，而是在描述一个工程结构。

## 推荐的理解方式

理解 `CMake` 最有效的方法，不是死记每个命令，而是抓住两个核心：

### 1. 先定义目标（target）

在现代 `CMake` 里，最重要的概念不是“全局变量”，而是“目标”。

目标可能是：

- 可执行文件
- 静态库
- 动态库
- 接口库
- 测试程序

### 2. 再把属性挂到目标上

例如：

- 头文件目录
- 编译选项
- 链接依赖
- 预处理宏

都尽量挂到具体目标上，而不是全局乱飞。

这就是现代 `CMake` 和很多旧式写法最大的差别。

----

# 一个更像真实工程的目录结构

先看一个常见布局：

```text
project/
├── CMakeLists.txt
├── include/
│   └── mylib/
│       └── math.h
├── src/
│   ├── CMakeLists.txt
│   ├── math.cpp
│   └── main.cpp
├── tests/
│   ├── CMakeLists.txt
│   └── math_test.cpp
└── third_party/
```

这个结构背后的思想是：

- 顶层 `CMakeLists.txt` 管整个项目
- `src/` 管业务实现和主目标
- `tests/` 管测试目标
- `include/` 放公共头文件
- `third_party/` 放外部依赖或相关配置

这比把所有内容堆在一个 `CMakeLists.txt` 里更适合长期维护。

----

# 顶层 CMakeLists.txt 应该做什么

> 顶层文件的职责通常不是“把所有细节都写完”，而是“定义全局框架，再把工作分发给子目录”。

一个比较清晰的顶层 `CMakeLists.txt` 可以这样写：

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build unit tests" ON)

add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
```

## 这几行各自在做什么

### `cmake_minimum_required()`

```cmake
cmake_minimum_required(VERSION 3.16)
```

声明最低 `CMake` 版本。  
这很重要，因为不同版本支持的特性不同。

### `project()`

```cmake
project(MyProject VERSION 1.0 LANGUAGES CXX)
```

声明：

- 项目名
- 项目版本
- 使用语言

### C++ 标准设置

```cmake
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

这表示：

- 使用 `C++11`
- 不允许退回更低标准
- 尽量避免依赖编译器扩展

### `option()`

```cmake
option(BUILD_TESTS "Build unit tests" ON)
```

这是一个开关。  
它允许你在配置项目时控制是否构建测试。

### `add_subdirectory()`

```cmake
add_subdirectory(src)
```

表示把 `src/` 子目录交给它自己的 `CMakeLists.txt` 继续处理。  
这就是多目录组织的核心手段之一。

----

# 在 src 目录里组织库和程序

很多项目更推荐把“核心逻辑”做成库，再让主程序去链接这个库。  
原因很简单：

- 逻辑更容易复用
- 更方便单元测试
- 主程序更薄
- 工程分层更清楚

例如，`src/CMakeLists.txt` 可以这样写：

```cmake
add_library(mylib STATIC
    math.cpp
)

target_include_directories(mylib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(app
    main.cpp
)

target_link_libraries(app PRIVATE
    mylib
)
```

## 这个结构为什么好

它表达了很清晰的工程关系：

- `mylib` 是一个可复用模块
- `app` 是最终程序
- `app` 依赖 `mylib`
- `mylib` 对外公开的头文件在 `include/`

这样以后如果你要再写一个：

- `tool`
- `bench`
- `demo`

都可以直接复用 `mylib`。

----

# 头文件目录怎么管理

## 推荐方式：让库自己声明它的公开头文件目录

例如：

```cmake
target_include_directories(mylib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
```

这里的 `PUBLIC` 非常关键，它意味着：

- `mylib` 自己编译时需要这个头文件目录
- 任何链接 `mylib` 的目标，也会自动继承这个头文件目录

这比在全局写：

```cmake
include_directories(include)
```

更现代，也更容易维护。

## 为什么不推荐全局 include

因为全局写法会带来几个问题：

- 谁真正依赖这个目录，不清楚
- 项目一大后，依赖关系会越来越模糊
- 某个目录被所有目标“无脑共享”，耦合会变重

现代 `CMake` 更强调：

**谁需要，谁声明。**

----

# 编译选项怎么管理

## 最基础做法

你可以对某个目标单独加编译选项：

```cmake
target_compile_options(mylib PRIVATE
    -Wall
    -Wextra
)
```

这表示：

- 给 `mylib` 增加警告选项
- 但这些选项不向外传播

## 预处理宏

如果某个目标需要特定宏，可以写：

```cmake
target_compile_definitions(mylib PRIVATE
    MYLIB_ENABLE_LOG=1
)
```

## Debug / Release 区分

有时不同构建类型需要不同选项：

```cmake
target_compile_definitions(mylib PRIVATE
    $<$<CONFIG:Debug>:MYLIB_DEBUG>
)
```

初学阶段不用一下子把这些生成器表达式全吃透，先知道：

- `Debug` 和 `Release` 可以走不同配置
- `CMake` 支持按配置切换属性

就够了。

## 一个实用原则

编译选项尽量做到：

- 有明确归属
- 少写全局魔法
- 能解释为什么存在

否则后面别人看到一堆选项，很难判断它们是给谁准备的。

----

# 第三方库怎么接入

> 这是很多项目从“小练习”走向“真实工程”的分水岭。

接入第三方库时，你通常要解决三件事：

1. 头文件怎么找到
2. 库怎么链接
3. 版本和来源怎么管理

## 最基础情况：系统已经安装好库

很多库可以通过 `find_package()` 查找：

```cmake
find_package(fmt REQUIRED)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE fmt::fmt)
```

这段配置表达的意思是：

- 查找 `fmt`
- 如果找不到就报错
- 找到之后，把 `fmt` 链接给 `app`

其中 `fmt::fmt` 是一个导入目标（imported target）。  
现代 `CMake` 很鼓励用这种方式，而不是手工写一堆库路径。

## 项目内直接包含第三方源码

如果你把某个依赖放进 `third_party/`，有时可以：

```cmake
add_subdirectory(third_party/somelib)
target_link_libraries(app PRIVATE somelib)
```

这种方式的好处是：

- 项目依赖更可控
- 新机器拉代码后更容易直接构建

但代价也很明显：

- 仓库变大
- 依赖升级成本更高

## 用 FetchContent 拉依赖

`CMake` 也支持在配置阶段拉取依赖：

```cmake
include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
)

FetchContent_MakeAvailable(fmt)
```

然后就可以：

```cmake
target_link_libraries(app PRIVATE fmt::fmt)
```

这种方式的优点是：

- 自动化程度高
- 不需要手工复制依赖源码

但也要注意：

- 构建依赖网络环境
- 版本要固定
- 团队和 CI 的可复现性要提前考虑

## 一个重要建议

无论怎么接第三方库，都尽量优先使用：

- `find_package()`
- 导入目标
- `target_link_libraries()`

而不是手工满项目写：

- `include_directories(...)`
- `link_directories(...)`
- 直接拼裸库文件路径

现代 `CMake` 的思想是让依赖关系显式、可传播、可维护。

----

# 测试怎么接入

## 为什么测试也应该进入构建系统

因为测试不是“构建完成后顺手跑一下”，而是项目结构的一部分。

把测试接入 `CMake` 的好处包括：

- 测试目标和主工程一起管理
- CI 更容易统一执行
- 测试依赖关系清晰
- 新人更容易知道“怎么跑测试”

## 最基础写法

假设 `tests/CMakeLists.txt`：

```cmake
add_executable(math_test
    math_test.cpp
)

target_link_libraries(math_test PRIVATE
    mylib
)

add_test(NAME math_test COMMAND math_test)
```

前提是顶层已经：

```cmake
enable_testing()
```

这样你就可以通过：

```bash
ctest --test-dir build
```

来运行测试。

## 为什么“核心逻辑做成库”更容易测

如果你的业务逻辑全塞在 `main.cpp` 里，会非常难测。  
而如果逻辑先抽到 `mylib` 里：

- 主程序只是调用
- 测试也可以直接链接 `mylib`

这就是为什么很多工程会坚持：

- 可执行目标尽量薄
- 业务逻辑优先沉到库中

----

# 安装怎么做

> 对练习项目来说，安装不是第一优先级；但一旦你在写库、工具链或可分发程序，安装就很重要。

## 基本安装命令

例如：

```cmake
install(TARGETS app
    RUNTIME DESTINATION bin
)
```

这表示：

- 安装 `app`
- 把可执行文件放到 `bin/`

如果你有库和头文件，也可以进一步写：

```cmake
install(TARGETS mylib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION include
)
```

## 有什么用

安装规则的意义在于：

- 统一产物布局
- 方便本地安装
- 为打包和发布提供基础

配置完后，通常可以执行：

```bash
cmake --install build
```

来把产物安装到目标位置。

----

# 多目录项目怎么拆

> 一个非常实用的经验：目录拆分应当反映模块边界，而不是为了“看起来高级”。

## 常见拆法

你可以按职责拆：

- `src/`：主源码
- `include/`：公共头文件
- `tests/`：测试
- `examples/`：示例
- `tools/`：工具程序

也可以按模块拆：

```text
src/
├── core/
├── net/
├── storage/
└── ui/
```

然后每个模块再有自己的 `CMakeLists.txt`。

## 什么时候该拆子目录

当出现下面情况时，通常就该考虑拆分：

- 一个 `CMakeLists.txt` 太长
- 目标数量变多
- 模块边界已经比较明确
- 不同目录有不同依赖

## 一个重要原则

**顶层负责结构，子目录负责细节。**

也就是说：

- 顶层别写太多具体源文件
- 子目录里再去定义库、程序、测试等细节

这样层次会清楚很多。

----

# 现代 CMake 的一些推荐实践

## 1. 优先 target 风格，而不是全局风格

更推荐：

```cmake
target_include_directories(...)
target_link_libraries(...)
target_compile_options(...)
target_compile_definitions(...)
```

少用：

```cmake
include_directories(...)
link_directories(...)
add_definitions(...)
```

## 2. 让可执行程序尽量薄

把核心逻辑做成库，主程序只做：

- 参数解析
- 初始化
- 调用业务入口

这样更容易复用，也更容易测试。

## 3. 头文件和实现目录尽量有清晰边界

例如：

- `include/mylib/...` 放对外头文件
- `src/...` 放实现

这样以后做安装、导出、打包都更自然。

## 4. 少写“神秘宏”

有些项目会封装很多自定义宏，把 `CMake` 写得像另一门 DSL。  
这在大型工程里有时有价值，但对大多数项目来说：

- 过度封装会让读者很难理解构建过程
- 错误排查难度也会上升

优先写清晰、标准、可解释的 `CMake`，通常更好。

## 5. 配置和构建分离

推荐使用：

```bash
cmake -S . -B build
cmake --build build
```

而不是在源码目录里到处生成中间文件。

## 6. 敢于重建构建目录

当配置缓存混乱、依赖变更较大时，删除 `build/` 重来往往比死磕更快。  
这不是“菜”，而是很正常的工程操作。

----

# 初学者最容易踩的坑

## 1. 头文件目录设置错了

表现通常是：

- `fatal error: xxx.h: No such file or directory`

优先检查：

- `target_include_directories()` 是否写对
- 头文件实际目录结构是否和 `#include` 方式匹配

## 2. 库链接漏了

表现通常是：

- `undefined reference`
- Windows 下的 `unresolved external symbol`

这说明编译可能通过了，但链接阶段没把库接上。

## 3. 公共依赖写成了 PRIVATE

例如：

- `mylib` 的公共头文件里用到了某依赖
- 但你把这个依赖只写成 `PRIVATE`

结果往往是：

- `mylib` 自己能编
- 依赖 `mylib` 的目标却编不过

这类问题的根源通常在于没有想清楚“接口依赖”和“实现依赖”的区别。

## 4. 把所有东西都写在顶层

小项目还能忍，大项目会迅速失控。  
一旦目标和目录变多，就应该拆分子目录。

## 5. 盲目复制别人的 CMake 片段

很多初学者遇到问题时，喜欢直接复制网上的配置。  
短期可能能跑，长期则会留下自己都解释不清的配置债务。

更稳妥的方式是：

- 先知道每一条命令在解决什么问题
- 再决定要不要加进项目

----

# 一个推荐的成长路径

如果你想真正掌握 `CMake`，可以按这个顺序学：

1. 先写一个最小 `CMakeLists.txt`，只构建单个可执行程序
2. 再把逻辑拆成库 + 可执行程序
3. 再引入 `include/` 和 `src/` 分离
4. 再拆成多目录 `CMakeLists.txt`
5. 再加入测试
6. 再接入第三方库
7. 最后再处理安装、导出、打包、CI

这个顺序的好处是：  
每一步都只新增一小层复杂度，不容易一下子被完整工程吓住。

----

# 小结

`CMake` 实战可以浓缩成一句话：

**不要把它当成“写命令的地方”，而要把它当成“描述工程结构与依赖关系的地方”。**

如果你只先记住最关键的几件事，可以抓住这些：

- 顶层 `CMakeLists.txt` 负责整体结构，子目录负责模块细节
- 优先定义库，再让程序和测试去链接库
- 头文件目录、编译选项、链接依赖尽量挂到具体 target 上
- 第三方库优先用 `find_package()`、导入目标和 `target_link_libraries()`
- 测试、安装、第三方依赖都应当被正式纳入构建系统

当你开始用 `target`、依赖传播、模块边界和构建目录分离的视角去写 `CMake`，你就已经从“能把项目跑起来”迈向了“能把项目长期维护下去”。
