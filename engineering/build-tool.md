# 构建工具

> 当 `C++` 项目只有一个文件时，手敲一条编译命令还不算痛苦。  
> 但只要文件一多、依赖一多、平台一多，*“怎么稳定地把项目构建出来”* 就会迅速变成一门单独的工程能力。

很多刚入门的读者会有类似困惑：

- 我已经会写 `g++ main.cpp` 了，为什么还需要 `make`
- `make` 和 `cmake` 到底是什么关系
- 它们都是“编译器”吗
- 为什么有些项目让我执行 `make`，有些项目让我先 `cmake ..` 再 `make`
- 为什么换一台机器，同样的项目就构建失败

> 编译器负责把代码翻译成目标程序  
> 而构建工具负责管理 `该编哪些文件、以什么顺序编、依赖怎么组织、库怎么接进来`

----

# 什么是构建

> “构建（build）” 不是单指“编译”。

在真实项目里，构建通常包括：

- 编译源文件
- 链接目标文件
- 处理依赖库
- 拷贝资源文件
- 生成可执行文件或库
- 运行测试
- 有时还包括打包、安装、部署

> “构建工具”不是在替代编译器，而是在 *组织整个构建流程*。

假设项目一开始只有一个文件: `g++ main.cpp -o app` 这没问题。

但项目很快可能会变成这样：

```text
project/
├── main.cpp
├── logger.cpp
├── logger.h
├── math.cpp
├── math.h
└── net.cpp
```

这时你可能要写：

```bash
g++ main.cpp logger.cpp math.cpp net.cpp -o app
```

再后来，你又需要：

- 指定头文件搜索路径
- 链接第三方库
- 区分 Debug / Release
- 支持 Windows 和 Linux
- 只重新编译改过的文件

如果每次都靠人手写命令，就会越来越难维护。

于是构建工具就出现了。它帮助你回答这些问题：

- 哪些文件参与构建
- 文件之间的依赖关系是什么
- 哪些目标需要先构建
- 改了一个文件后，哪些内容需要重新生成
- 第三方库怎么接入

----

# 构建工具和编译器

> 一个非常重要的区分：`make` 和 `cmake` 都不是编译器。

常见角色可以这样理解：

- **编译器**：如 `g++`、`clang++`、`cl`，负责真正翻译代码
- **链接器**：负责把目标文件和库拼装起来
- **构建工具**：负责组织整个流程

例如：

- `g++` 知道怎么编译一个 `.cpp`
- `make` 知道“这个项目有哪些目标、哪些文件依赖哪些文件”
- `cmake` 知道“如何为不同平台生成对应的构建系统工程”

所以不要把它们理解成同一层东西。

----

# Make

> `make` 的核心目标，是解决 *哪些东西需要重新生成* 这个问题。

在大型项目里，如果你每改一个文件就把整个工程全部重编译，会非常慢。`make` 的思路是：

- 先描述目标和依赖关系
- 再根据文件时间戳判断哪些内容过期了
- 只执行必要的那部分命令

> 这就是 `make` 最经典的价值：**增量构建**。

你只需要先理解三个词：

- `target`：目标，也就是你想生成的东西
- `dependency`：依赖，也就是目标依赖哪些文件
- `rule`：规则，也就是如何从依赖生成目标

## 最小示例

```text
project/
├── main.cpp
├── math.cpp
├── math.h
└── Makefile
```

代码可能是这样：

```cpp
// math.h
#ifndef MATH_H
#define MATH_H

int add(int a, int b);

#endif
```

```cpp
// math.cpp
#include "math.h"

int add(int a, int b) {
    return a + b;
}
```

```cpp
// main.cpp
#include <iostream>
#include "math.h"

int main() {
    std::cout << add(3, 4) << "\n";
    return 0;
}
```

对应的 `Makefile` 可以写成：

```make
app: main.o math.o
	g++ main.o math.o -o app

main.o: main.cpp math.h
	g++ -c main.cpp -o main.o

math.o: math.cpp math.h
	g++ -c math.cpp -o math.o

clean:
	rm -f app main.o math.o
```

!> **注意：命令前面必须使用 `Tab 字符`，而不能是空格。**

### Makefile 解读

```make
app: main.o math.o
	g++ main.o math.o -o app
```

只要 `app` 不存在，或者 `main.o` / `math.o` 更新了，就重新链接。

```make
main.o: main.cpp math.h
	g++ -c main.cpp -o main.o
```

只要 `main.cpp` 或 `math.h` 变了，就重新编译 `main.o`。

```make
math.o: math.cpp math.h
	g++ -c math.cpp -o math.o
```

只要 `math.cpp` 变了，就重新编译 `math.o`。

```make
clean:
	rm -f app main.o math.o
```

清理构建产物

### 使用方法

最常见的命令：

```bash
make
```

如果 `Makefile` 里第一条规则是 `app`，那么直接执行 `make` 时，默认就会尝试构建这个目标。

如果你想指定目标：

```bash
make clean
make app
```

## 优点和局限

### 优点

- 概念直接
- 对小项目非常够用
- 增量构建思路清晰
- 在 Unix/Linux 环境中很常见
- 许多底层项目和老项目仍大量使用

### 局限

`make` 很强，但它的问题也很现实：

- 语法不够现代
- 跨平台体验一般
- 手工维护依赖关系容易出错
- 大项目的 `Makefile` 容易越来越难读
- 第三方库和复杂工程配置管理不够友好

换句话说，`make` 很适合帮助你理解“构建是什么”，但在现代跨平台工程里，它常常不再是最高层选择。

----

# Makefile 配置详解

一条规则通常长这样：

```make
target: dependencies
	command
```

注意有几个关键点：

- `target` 在冒号左边
- `dependencies` 在冒号右边
- **下一行命令前面必须是 `Tab`，不是空格**

## 变量

`Makefile` 支持变量，方便复用：

```make
CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2

app: main.o math.o
	$(CXX) main.o math.o -o app

main.o: main.cpp math.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

math.o: math.cpp math.h
	$(CXX) $(CXXFLAGS) -c math.cpp -o math.o
```

这样做的好处是：

- 编译器改起来方便
- 编译选项集中管理
- 项目更清晰

## 伪目标 .PHONY

像 `clean` 这样的目标，并不是真的去生成一个叫 `clean` 的文件。  
这种目标通常应该声明为伪目标：

```make
.PHONY: clean

clean:
	rm -f app main.o math.o
```

> 如果当前目录下刚好真的有个文件叫 `clean`，`make clean` 可能会误以为目标已经“最新”，从而不执行命令。

## 自动变量

`make` 里有一些常用自动变量：

- `$@`：当前目标名
- `$<`：第一个依赖
- `$^`：所有依赖

例如可以写成：

```make
CXX = g++
CXXFLAGS = -std=c++11 -Wall

app: main.o math.o
	$(CXX) $^ -o $@

main.o: main.cpp math.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

math.o: math.cpp math.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
```

这比把文件名反复手写更不容易出错。

## 模式规则

如果项目里有很多 `.cpp`，一条条写规则会很重复。  
这时可以使用模式规则：

```make
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
```

它表示：

- 任何 `.o` 都可以由对应同名 `.cpp` 生成

例如：

- `main.o` <- `main.cpp`
- `math.o` <- `math.cpp`

这是 `Makefile` 从“能用”走向“更像工程”的重要一步。

----

# CMake

> `cmake` 不是 `make` 的替代品那么简单，它更像是 *生成构建系统的工具*。

很多初学者第一次看到：

```bash
cmake -S . -B build
cmake --build build
```

会误以为 `cmake` 就是在直接编译代码。更准确地说，`cmake` 通常分两步工作：

1. 读取 `CMakeLists.txt`
2. 为当前平台生成合适的构建系统

这个“构建系统”可以是：

- Unix Makefiles
- Ninja
- Visual Studio 工程
- Xcode 工程

> 也就是说，`cmake` 更像是 *构建系统生成器（build system generator）*。

## 流行原因

因为真实工程往往有这些需求：

- 同一套源码在 Windows / Linux / macOS 上构建
- 支持不同编译器
- 管理多个目标：可执行文件、静态库、动态库、测试
- 管理第三方库
- 统一编译选项

如果全部靠手写 `Makefile` 来兼顾这些需求，工作量会迅速上升。  

> `cmake` 的价值就在于：*用更抽象的方式描述项目，再让它生成具体平台上的构建文件。*

## 最小示例

还是用刚才那个小项目：

```text
project/
├── CMakeLists.txt
├── main.cpp
├── math.cpp
└── math.h
```

一个最小 `CMakeLists.txt` 可以写成：

```cmake
cmake_minimum_required(VERSION 3.10)
project(HelloCMake)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app
    main.cpp
    math.cpp
)
```

这几行就已经表达了很多信息：

- 这个项目最低需要什么版本的 `cmake`
- 项目名是什么
- 使用 `C++11`
- 生成一个叫 `app` 的可执行文件
- 这个目标由哪些源文件组成

### 如何构建

常见做法是使用单独的 `build/` 目录：

```bash
cmake -S . -B build
cmake --build build
```

第一条命令的意思大致是：

- `-S .`：源码目录是当前目录
- `-B build`：构建目录是 `build`

它会在 `build/` 下生成对应平台的构建文件。

第二条命令则告诉 `cmake`：

- 去使用 `build/` 里的构建系统真正执行构建

### 为什么推荐单独的 build 目录

因为这样可以做到：

- 源码目录干净
- 构建产物集中
- 方便整体删除重建
- 同一份源码可以对应多个构建目录

例如：

- `build-debug/`
- `build-release/`

----

# CMakeLists.txt 配置详解

> `CMake` 的很多命令看起来像“函数调用”，其实是在 *声明一个构建目标的属性和依赖关系*。

如果只记住“这行能跑”，但不知道参数分别代表什么、会影响谁、是否会向下游传递，到了真实项目里就很容易改不动。

下面按“真实项目使用频率”来梳理。

## 最常见的命令

### 声明 CMake 最小版本： cmake_minimum_required()

```cmake
cmake_minimum_required(VERSION 3.16)
```

作用是声明：这个项目要求的最低 `CMake` 版本是多少。

这里的参数含义是：

- `VERSION`：后面跟的是最低版本号
- `3.16`：表示低于这个版本的 `CMake` 不应该用来配置这个项目

为什么它重要？

- 不同版本 `CMake` 的特性不同
- 一些现代写法依赖较新的行为
- 先把最低版本说清楚，能减少“同样的脚本在别人机器上失效”的问题

它通常放在 `CMakeLists.txt` 的最前面。

### 声明项目信息： project()

```cmake
project(MyProject VERSION 1.0 LANGUAGES CXX)
```

它声明项目的基本信息。

常见参数含义：

- `MyProject`：项目名
- `VERSION 1.0`：项目版本
- `LANGUAGES CXX`：这个项目主要使用 `C++`

最小形式也可以只写：

```cmake
project(MyProject)
```

但真实项目里写清版本和语言通常更规范。

### 设置变量： set()

```cmake
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

`set()` 的作用非常通用，本质上是在设置变量。

这两行里每个字段的含义是：

- `CMAKE_CXX_STANDARD`：一个 `CMake` 变量名
- `17`：把这个变量设置为 `17`

它表达的目标是：

- 希望当前项目使用 `C++17`

第二行：

- `CMAKE_CXX_STANDARD_REQUIRED`：表示这个标准是不是“必须满足”
- `ON`：要求编译器必须支持这个标准，而不是自动降级

对初学者来说，`set()` 最常见的用途有三种：

- 设置语言标准
- 设置一些开关变量
- 保存路径、输出目录等配置

不过要注意，`set()` 是 **变量级别** 的配置，而现代 `CMake` 更提倡把很多属性挂到具体 target 上。

### 定义一个可执行目标： add_executable()

```cmake
add_executable(app main.cpp math.cpp)
```

它的作用是定义一个可执行目标。

这里的参数可以逐个理解：

- `app`：目标名
- `main.cpp math.cpp`：组成这个目标的源文件列表

它表达的目标是：

- 生成一个叫 `app` 的可执行程序
- 这个程序由 `main.cpp` 和 `math.cpp` 一起编译、链接而成

在真实项目里更常见的写法通常会分行写，便于维护：

```cmake
add_executable(app
    src/main.cpp
    src/math.cpp
)
```

### 构建库： add_library()

```cmake
add_library(mathlib STATIC math.cpp)
```

这一行一定要彻底看懂，因为它是很多工程拆分的基础。

参数含义如下：

- `mathlib`：目标名，也就是这个库在 `CMake` 里的名字
- `STATIC`：库类型，表示静态库
- `math.cpp`：这个库由哪些源文件构成

这行命令实现的目标是：

- 把 `math.cpp` 先编译成一个独立的库目标
- 这个库目标名字叫 `mathlib`
- 库的形态是静态库，后续其他目标可以链接它

常见库类型包括：

- `STATIC`：静态库，链接时直接并入最终产物
- `SHARED`：动态库，运行时通常需要配套的 `.so` / `.dll`
- `INTERFACE`：接口库，不生成真正的二进制文件，只用来传递头文件路径、宏、编译选项等属性

例如：

```cmake
add_library(mathlib STATIC
    src/math.cpp
)

add_executable(app
    src/main.cpp
)

target_link_libraries(app PRIVATE
    mathlib
)
```

这几行组合起来的含义是：

- 先把 `src/math.cpp` 构建成静态库 `mathlib`
- 再构建可执行程序 `app`
- `app` 链接 `mathlib`

这就是库与可执行程序之间最基本的工程关系。

### 给目标添加头文件搜索路径： target_include_directories()

```cmake
target_include_directories(app PRIVATE include)
```

这条命令的作用是：给某个目标添加头文件搜索路径。

参数含义：

- `app`：这个配置作用到哪个目标上
- `PRIVATE`：只对 `app` 自己生效，不向依赖它的其他目标传递
- `include`：要加入的头文件搜索目录

它实现的目标是：

- 当编译 `app` 时，编译器会额外去 `include/` 目录里找头文件

如果你写的是：

```cmake
target_include_directories(mathlib PUBLIC include)
```

那意思就变成：

- `mathlib` 自己编译时需要 `include/`
- 任何链接 `mathlib` 的目标，也会自动继承这个头文件路径

> 这在库项目里非常常见，因为库的头文件通常就是要暴露给使用者的。

### 构建链接关系： target_link_libraries()

```cmake
target_link_libraries(app PRIVATE mathlib)
```

> 这是 `CMake` 中最重要的命令之一。

参数含义：

- `app`：要链接谁
- `PRIVATE`：这条链接关系只对 `app` 自己生效
- `mathlib`：被链接的库目标

它实现的目标是：

- 告诉 `CMake`，构建 `app` 时需要把 `mathlib` 一起链接进来

如果这里链接的是第三方库，写法也类似：

```cmake
target_link_libraries(app PRIVATE fmt::fmt)
```

这里的 `fmt::fmt` 往往是第三方库导出的 target 名，而不是文件路径。

### PRIVATE / PUBLIC / INTERFACE

> 这是现代 `CMake` 的核心概念之一，可以理解成“一个属性是否向外传播”。

- `PRIVATE`：当前目标自己使用，不向外传递
- `PUBLIC`：当前目标使用，也向依赖它的目标传递
- `INTERFACE`：当前目标自己不用，但向依赖它的目标传递

最容易理解的例子是头文件路径：

```cmake
target_include_directories(mathlib PUBLIC include)
```

意思是：

- `mathlib` 自己需要这个头文件目录
- 任何链接 `mathlib` 的目标，也需要这个头文件目录

而下面这种通常更适合“只在库内部使用的实现细节目录”：

```cmake
target_include_directories(mathlib PRIVATE src)
```

这表示：

- `src/` 只是 `mathlib` 内部实现需要
- 外部使用者不应该看到这个路径

这就是现代 `CMake` 的核心思想：**把依赖、头文件、宏、编译选项都挂到具体 target 上，并且明确它们是否传播。**

## 真实项目里高频命令

### 子目录构建： add_subdirectory()

```cmake
add_subdirectory(src)
add_subdirectory(tests)
```

作用是把子目录里的 `CMakeLists.txt` 纳入当前构建。

真实项目常用它来做两件事：

- 拆分大型工程
- 把第三方库源码作为子目录引入

例如：

```cmake
add_subdirectory(third_party/spdlog)
target_link_libraries(app PRIVATE spdlog::spdlog)
```

这表示：

- `spdlog` 的源码也参加当前工程构建
- `app` 链接它导出的目标

### 补充源文件： target_sources()

```cmake
target_sources(app PRIVATE
    src/main.cpp
    src/app.cpp
)
```

作用是给一个已经存在的 target 继续补充源文件。

它适合：

- 先定义目标，再分阶段添加源文件
- 在不同平台下给同一个目标追加不同文件

### 给目标添加预处理宏： target_compile_definitions()

```cmake
target_compile_definitions(app PRIVATE DEBUG_MODE=1)
```

作用是给目标添加预处理宏。

它相当于为这个目标增加类似：

```bash
-DDEBUG_MODE=1
```

的编译参数。

真实项目里常用来控制：

- Debug / Release 行为
- 平台分支
- 特性开关

### 为目标添加编译器选项： target_compile_options()

```cmake
target_compile_options(app PRIVATE -Wall -Wextra)
```

作用是为目标添加编译器选项。

常见用途：

- 打开警告
- 开启更严格的诊断
- 为某个特定目标定制编译参数

注意它更偏“编译器参数层面”，而不是“项目逻辑层面”。

### 声明这个目标需要哪些语言特性： target_compile_features()

```cmake
target_compile_features(app PRIVATE cxx_std_17)
```

它的作用是声明这个目标需要哪些语言特性。

相较于直接硬写编译器参数，这种方式更符合 `CMake` 的抽象思路，因为你描述的是“需要 `C++17` 能力”，而不是“强行传某个平台的某个开关”。

### 定义一个布尔开关： option()

```cmake
option(BUILD_TESTS "Build unit tests" ON)
```

作用是定义一个布尔开关，方便用户在配置阶段选择是否启用某项功能。

真实项目中非常常见，例如：

- 是否构建测试
- 是否启用示例程序
- 是否打开日志
- 是否使用某个可选依赖

### find_package()

```cmake
find_package(fmt REQUIRED)
```

这是接第三方库时最常见的一条命令。

参数含义：

- `fmt`：要查找的包名
- `REQUIRED`：如果找不到就直接报错，停止配置

它实现的目标是：

- 让 `CMake` 去当前环境里寻找 `fmt`
- 如果找到，就导入它提供的 target、头文件、库信息

后面通常会配合：

```cmake
target_link_libraries(app PRIVATE fmt::fmt)
```

这表示：

- 不是手动写 `.lib` 或 `.so` 路径
- 而是直接使用这个包导出的 `CMake target`

这也是现代 `CMake` 推荐的第三方库接入方式。

### 定义“安装规则”： install()

```cmake
install(TARGETS app DESTINATION bin)
```

作用是定义“安装规则”。

它不是在编译时马上执行，而是在你运行安装步骤时告诉 `CMake`：

- 可执行文件放到哪里
- 库放到哪里
- 头文件放到哪里

如果项目需要发布、打包或给别人复用，这一步很常见。

### 测试相关： enable_testing() / add_test()

```cmake
enable_testing()
add_test(NAME app_test COMMAND app_test)
```

这组命令用于测试。

作用分别是：

- `enable_testing()`：启用测试支持
- `add_test()`：注册一条测试命令

如果项目里已经引入了 `GoogleTest`、`Catch2` 之类的框架，这两条通常都会出现。

## 第三方库接入

### 方式一：系统或包管理器已安装，用 find_package()

这是最常见、也最推荐的方式。

```cmake
find_package(fmt REQUIRED)

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE fmt::fmt)
```

整个流程的意思是：

- 先找到 `fmt`
- 再把 `fmt::fmt` 链接到 `app`
- `fmt` 所需要的头文件路径、库路径、编译属性通常会自动传递过来

这也是为什么现代 `CMake` 里经常看不到显式写 `include_directories()`，因为很多信息已经由库自己的 target 携带了。

### 方式二：第三方库源码放在仓库里，用 add_subdirectory()

```cmake
add_subdirectory(third_party/spdlog)

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE spdlog::spdlog)
```

这适用于：

- 依赖库本身支持 `CMake`
- 你希望它和主项目一起构建
- 团队希望减少“本机先装依赖”的步骤

### 方式三：配置阶段自动下载，用 FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
)

FetchContent_MakeAvailable(fmt)

target_link_libraries(app PRIVATE fmt::fmt)
```

这适用于：

- 想降低新机器搭环境的门槛
- 希望项目配置时自动拉依赖

但对初学者来说，要先把 `find_package()` 和 `add_subdirectory()` 看明白，再接触 `FetchContent` 会更稳。

## 完整例子

目录结构：

```text
project/
├── CMakeLists.txt
├── include/
│   └── math.h
├── src/
│   ├── main.cpp
│   └── math.cpp
└── tests/
    ├── CMakeLists.txt
    └── test_math.cpp
```

对应的顶层 `CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build unit tests" ON)

add_library(mathlib STATIC
    src/math.cpp
)

target_include_directories(mathlib PUBLIC
    include
)

target_compile_features(mathlib PUBLIC cxx_std_17)

add_executable(app
    src/main.cpp
)

target_link_libraries(app PRIVATE
    mathlib
)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
```

`tests/CMakeLists.txt` 可以写成：

```cmake
add_executable(mathlib_test
    test_math.cpp
)

target_link_libraries(mathlib_test PRIVATE
    mathlib
)

add_test(NAME mathlib_test COMMAND mathlib_test)
```

这里的含义是：

- 生成一个测试程序 `mathlib_test`
- 这个测试程序链接主工程里的 `mathlib`
- 再把它注册成一条可以由 `ctest` 执行的测试

最小的 `tests/test_math.cpp` 可以写成：

```cpp
#include "math.h"
#include <cassert>

int main() {
    assert(add(1, 2) == 3);
    return 0;
}
```

这个例子虽然简单，但已经把测试子目录的最基本结构说明白了：

- 顶层 `CMakeLists.txt` 决定是否启用测试
- `tests/CMakeLists.txt` 负责定义测试目标
- `add_test()` 负责把测试目标接入 `CTest`

这份配置里，几个关键点分别是：

- `cmake_minimum_required()`：约束 `CMake` 版本
- `project()`：声明项目名、版本和语言
- `set()`：指定 `C++17`
- `option()`：定义是否构建测试的开关
- `add_library()`：把功能代码抽成库
- `target_include_directories()`：暴露库的头文件目录
- `target_compile_features()`：声明语言能力要求
- `add_executable()`：定义最终程序
- `target_link_libraries()`：把程序和库连接起来
- `add_subdirectory()`：在需要时引入测试子目录

如果你把这一套理解透，已经足够读懂很多真实项目的 `CMakeLists.txt` 了。

----

# Make 和 CMake 的关系

> 它们不是简单的 *二选一*，而是处在不同层次。

- `make` 是一个具体的构建执行工具
- `cmake` 是一个生成构建系统的工具

在 Linux 上很常见的一种流程是：

1. 写 `CMakeLists.txt`
2. 用 `cmake` 生成 `Makefile`
3. 再由 `make` 执行实际构建

也就是说，有时你看到项目里既有 `cmake`，又执行了 `make`，这并不矛盾。

## 一个形象比喻

可以把它们类比成：

- `cmake`：项目经理，先出施工图
- `make`：施工队，按图纸实际干活
- `g++`：具体工人，负责真正的加工

这个比喻不完全精确，但对初学者很有帮助。

## 最佳实践

对现代跨平台 `C++` 项目来说，通常更推荐：

- 学会阅读和编写基础 `Makefile`
- 但实际工程优先掌握 `CMake`

原因很简单：

- `Makefile` 能帮你理解构建依赖和增量构建本质
- `CMake` 更适合现代项目、跨平台、团队协作和第三方依赖管理

----

# 注意事项

### 1. 构建脚本也是项目源码的一部分

很多初学者只重视 `.cpp/.h`，却把 `Makefile`、`CMakeLists.txt` 当成 *附属配置*。  
实际上它们决定了项目能不能被别人正确构建出来。

所以构建脚本也应当：

- 可读
- 有结构
- 命名清晰
- 尽量少写魔法逻辑

### 2. 不要把所有配置写成全局

尤其在 `CMake` 中，现代实践更推荐：

- 使用 target 级别的配置
- 少用全局 `include_directories()`
- 少用全局 `link_libraries()`

因为 target 级配置更清楚地表达了“谁依赖谁”。

### 3. 头文件路径和链接库是两回事

很多人第一次接第三方库时会犯这个错：

- 头文件能找到，就以为万事大吉

其实还要确认：

- 是否正确链接库
- 动态库运行时是否能被找到

#$# 4. Debug 和 Release 要分开看

不同构建类型可能意味着：

- 优化等级不同
- 调试信息不同
- 宏定义不同
- 运行结果不同

所以不要拿 Debug 通过就直接等价成 Release 没问题。

### 5. 构建目录坏了就敢于重建

这是非常实用的经验：

- 构建目录只是中间产物
- 如果状态混乱，删掉重建通常比硬修更省时间

例如：

```bash
rm -rf build
cmake -S . -B build
cmake --build build
```

当然，在 Windows 上命令形式会不同，但思路一样。

### 6. 先让流程简单可解释，再追求高级技巧

很多初学者一上来就接触：

- 超复杂 `CMake` 宏
- 自动下载依赖
- 大量平台分支
- 复杂生成脚本

这会非常容易迷失。  
更稳妥的方式是：

- 先把一个最小项目的构建链路跑通
- 再逐步增加库、目录、多目标、测试

----

# 小结

构建工具这件事，可以浓缩成一句话：

**编译器负责“把代码翻译出来”，构建工具负责“把整个项目稳定地组织出来”。**

如果你只先记住最关键的几件事，可以抓住这些：

- `make` 通过“目标、依赖、规则”来管理构建
- `Makefile` 很适合理解增量构建和依赖关系
- `cmake` 更像构建系统生成器，适合现代跨平台项目
- `cmake` 和 `make` 不是对立关系，很多时候它们会一起出现
- 对零基础来说，先理解手工编译，再学 `make`，最后学 `CMake`，是最顺的路径

当你开始把构建脚本也当成工程设计的一部分，而不只是“为了让代码跑起来的附属物”，你就真正进入了现代 `C++` 工程开发的世界。
