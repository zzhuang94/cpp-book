# 构建工具

> 当 `C++` 项目只有一个文件时，手敲一条编译命令还不算痛苦。  
> 但只要文件一多、依赖一多、平台一多，**“怎么稳定地把项目构建出来”** 就会迅速变成一门单独的工程能力。

很多刚入门的读者会有类似困惑：

- 我已经会写 `g++ main.cpp` 了，为什么还需要 `make`
- `make` 和 `cmake` 到底是什么关系
- 它们都是“编译器”吗
- 为什么有些项目让我执行 `make`，有些项目让我先 `cmake ..` 再 `make`
- 为什么换一台机器，同样的项目就构建失败

这些问题都很正常。  
因为在 `C++` 里，**编译器负责把代码翻译成目标程序，而构建工具负责管理“该编哪些文件、以什么顺序编、依赖怎么组织、库怎么接进来”**。

这篇文档把读者默认当作零基础来介绍。你会看到：

- 什么是构建工具，为什么不能只靠手写编译命令
- `make` 的核心思想：目标、依赖、规则
- `Makefile` 最基础的写法和常见概念
- `cmake` 的定位、它与 `make` 的关系
- 如何用 `CMakeLists.txt` 从零构建一个小型项目
- 开发过程中的常见坑和实践建议

----

# 先回答一个根本问题

## 什么是构建

“构建（build）” 不是单指“编译”。

在真实项目里，构建通常包括：

- 编译源文件
- 链接目标文件
- 处理依赖库
- 拷贝资源文件
- 生成可执行文件或库
- 运行测试
- 有时还包括打包、安装、部署

所以“构建工具”不是在替代编译器，而是在**组织整个构建流程**。

## 为什么不能一直手写命令

假设项目一开始只有一个文件：

```bash
g++ main.cpp -o app
```

这没问题。

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

# 构建工具和编译器的关系

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

# 为什么会有 `make`

> `make` 的核心目标，是解决“哪些东西需要重新生成”这个问题。

在大型项目里，如果你每改一个文件就把整个工程全部重编译，会非常慢。  
`make` 的思路是：

- 先描述目标和依赖关系
- 再根据文件时间戳判断哪些内容过期了
- 只执行必要的那部分命令

这就是 `make` 最经典的价值：**增量构建**。

## `make` 的核心思想

你只需要先理解三个词：

- `target`：目标，也就是你想生成的东西
- `dependency`：依赖，也就是目标依赖哪些文件
- `rule`：规则，也就是如何从依赖生成目标

一个最经典的 `Makefile` 规则长这样：

```make
app: main.o logger.o
	g++ main.o logger.o -o app
```

意思是：

- 目标是 `app`
- 它依赖 `main.o` 和 `logger.o`
- 如果要生成 `app`，就执行后面那条命令

如果 `main.o` 或 `logger.o` 比 `app` 更新，`make` 就会重新执行这条规则。

----

# Make 是什么

## 一个最小例子

先看一个非常小的项目：

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

## 这份 Makefile 在做什么

它定义了四件事：

### 1. 如何生成最终程序

```make
app: main.o math.o
	g++ main.o math.o -o app
```

只要 `app` 不存在，或者 `main.o` / `math.o` 更新了，就重新链接。

### 2. 如何生成 `main.o`

```make
main.o: main.cpp math.h
	g++ -c main.cpp -o main.o
```

只要 `main.cpp` 或 `math.h` 变了，就重新编译 `main.o`。

### 3. 如何生成 `math.o`

```make
math.o: math.cpp math.h
	g++ -c math.cpp -o math.o
```

### 4. 如何清理构建产物

```make
clean:
	rm -f app main.o math.o
```

## 怎么使用

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

----

# Makefile 的基本语法

## 规则的基本形式

一条规则通常长这样：

```make
target: dependencies
	command
```

注意有几个关键点：

- `target` 在冒号左边
- `dependencies` 在冒号右边
- 下一行命令前面必须是 **Tab**，不是空格

这个 “Tab” 是 `Makefile` 最经典的新手坑之一。

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

## 伪目标 `.PHONY`

像 `clean` 这样的目标，并不是真的去生成一个叫 `clean` 的文件。  
这种目标通常应该声明为伪目标：

```make
.PHONY: clean

clean:
	rm -f app main.o math.o
```

为什么需要这个？

因为如果当前目录下刚好真的有个文件叫 `clean`，`make clean` 可能会误以为目标已经“最新”，从而不执行命令。

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

# Make 的优点和局限

## 优点

- 概念直接
- 对小项目非常够用
- 增量构建思路清晰
- 在 Unix/Linux 环境中很常见
- 许多底层项目和老项目仍大量使用

## 局限

`make` 很强，但它的问题也很现实：

- 语法不够现代
- 跨平台体验一般
- 手工维护依赖关系容易出错
- 大项目的 `Makefile` 容易越来越难读
- 第三方库和复杂工程配置管理不够友好

换句话说，`make` 很适合帮助你理解“构建是什么”，但在现代跨平台工程里，它常常不再是最高层选择。

----

# 为什么会有 CMake

> `cmake` 不是 `make` 的替代品那么简单，它更像是“生成构建系统的工具”。

很多初学者第一次看到：

```bash
cmake -S . -B build
cmake --build build
```

会误以为 `cmake` 就是在直接编译代码。  
更准确地说，`cmake` 通常分两步工作：

1. 读取 `CMakeLists.txt`
2. 为当前平台生成合适的构建系统

这个“构建系统”可以是：

- Unix Makefiles
- Ninja
- Visual Studio 工程
- Xcode 工程

也就是说，`cmake` 更像是**构建系统生成器（build system generator）**。

## 它为什么流行

因为真实工程往往有这些需求：

- 同一套源码在 Windows / Linux / macOS 上构建
- 支持不同编译器
- 管理多个目标：可执行文件、静态库、动态库、测试
- 管理第三方库
- 统一编译选项

如果全部靠手写 `Makefile` 来兼顾这些需求，工作量会迅速上升。  
`cmake` 的价值就在于：**用更抽象的方式描述项目，再让它生成具体平台上的构建文件。**

----

# CMake 是什么

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

## 如何构建

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

# CMakeLists.txt 的核心概念

## 1. `project()`

```cmake
project(MyProject)
```

它声明项目信息。  
一般至少会写项目名，有时也会写版本、语言等信息。

## 2. `set()`

```cmake
set(CMAKE_CXX_STANDARD 11)
```

用来设置变量。  
对初学者来说，最常见的是设置：

- `CMAKE_CXX_STANDARD`
- 编译选项
- 路径变量

## 3. `add_executable()`

```cmake
add_executable(app main.cpp math.cpp)
```

表示定义一个可执行目标。

## 4. `add_library()`

```cmake
add_library(mathlib STATIC math.cpp)
```

表示定义一个库目标。  
常见类型包括：

- `STATIC`
- `SHARED`

例如：

```cmake
add_library(mathlib STATIC math.cpp)
add_executable(app main.cpp)
target_link_libraries(app PRIVATE mathlib)
```

这表示：

- 先把 `math.cpp` 构建成静态库 `mathlib`
- 再构建 `app`
- 让 `app` 链接 `mathlib`

## 5. `target_include_directories()`

如果头文件不在当前默认搜索路径里，可以这样指定：

```cmake
target_include_directories(app PRIVATE include)
```

这表示 `app` 构建时需要把 `include/` 作为头文件搜索目录。

## 6. `target_link_libraries()`

这是 `CMake` 中非常重要的一条命令：

```cmake
target_link_libraries(app PRIVATE mathlib)
```

它表示：

- `app` 需要链接 `mathlib`

这个命令也经常用于链接第三方库。

## 7. `PRIVATE / PUBLIC / INTERFACE`

这是 `CMake` 里非常关键，但初学者一开始最容易迷糊的概念。

可以先这样粗略理解：

- `PRIVATE`：只对当前目标自己生效
- `PUBLIC`：对当前目标生效，也会传递给依赖它的目标
- `INTERFACE`：当前目标自己不用，但依赖它的目标要用

例如：

```cmake
target_include_directories(mathlib PUBLIC include)
```

这表示：

- `mathlib` 自己需要 `include/`
- 链接 `mathlib` 的其他目标，也会继承这个头文件路径

这是现代 `CMake` 的核心思想之一：  
**把依赖和属性挂到具体 target 上，而不是到处写全局配置。**

----

# 一个更像真实项目的 CMake 例子

目录结构：

```text
project/
├── CMakeLists.txt
├── include/
│   └── math.h
└── src/
    ├── main.cpp
    └── math.cpp
```

头文件：

```cpp
// include/math.h
#ifndef MATH_H
#define MATH_H

int add(int a, int b);

#endif
```

实现：

```cpp
// src/math.cpp
#include "math.h"

int add(int a, int b) {
    return a + b;
}
```

主程序：

```cpp
// src/main.cpp
#include <iostream>
#include "math.h"

int main() {
    std::cout << add(1, 2) << "\n";
    return 0;
}
```

对应的 `CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(mathlib STATIC
    src/math.cpp
)

target_include_directories(mathlib PUBLIC
    include
)

add_executable(app
    src/main.cpp
)

target_link_libraries(app PRIVATE
    mathlib
)
```

这份配置已经体现了现代 `CMake` 的基本风格：

- 先定义目标
- 再把头文件路径、链接关系等属性挂到目标上

----

# Make 和 CMake 的关系

> 它们不是简单的“二选一”，而是处在不同层次。

可以这样理解：

- `make` 是一个具体的构建执行工具
- `cmake` 是一个生成构建系统的工具

在 Linux 上很常见的一种流程是：

1. 写 `CMakeLists.txt`
2. 用 `cmake` 生成 `Makefile`
3. 再由 `make` 执行实际构建

也就是说，有时你看到项目里既有 `cmake`，又执行了 `make`，这并不矛盾。

## 一个形象比喻

你可以把它们类比成：

- `cmake`：项目经理，先出施工图
- `make`：施工队，按图纸实际干活
- `g++`：具体工人，负责真正的加工

这个比喻不完全精确，但对初学者很有帮助。

## 现代实践里更推荐什么

对现代跨平台 `C++` 项目来说，通常更推荐：

- 学会阅读和编写基础 `Makefile`
- 但实际工程优先掌握 `CMake`

原因很简单：

- `Makefile` 能帮你理解构建依赖和增量构建本质
- `CMake` 更适合现代项目、跨平台、团队协作和第三方依赖管理

----

# 初学者最常见的问题

## 1. 把 `cmake` 当成编译器

不是。  
`cmake` 更像“生成构建方案”的工具，真正的编译工作仍由编译器完成。

## 2. 以为 `make` 只能用于 C++

也不是。  
`make` 本质上是一个通用规则执行工具，只是经常被用在 `C/C++` 项目中。

## 3. 不理解源码目录和构建目录分离

初学者很容易直接在源码目录执行一堆构建命令，结果目录里到处都是：

- `.o`
- 可执行文件
- 中间缓存
- 自动生成文件

更推荐的做法是：

```bash
cmake -S . -B build
cmake --build build
```

把生成物都放在 `build/`。

## 4. 只会“复制命令”，不会读错误

构建工具报错时，不要只盯着最后一行。  
要学会区分：

- 配置错误
- 编译错误
- 链接错误
- 运行时错误

例如：

- `fatal error: xxx.h: No such file or directory`：通常是头文件路径问题
- `undefined reference to ...`：通常是链接问题
- `No rule to make target ...`：通常是 `Makefile` 依赖或路径问题

## 5. 不知道应该从哪里学起

很实用的学习顺序是：

1. 先手写一条最简单的 `g++` 编译命令
2. 理解一个小型 `Makefile` 的目标、依赖、规则
3. 再学最小 `CMakeLists.txt`
4. 再理解库、头文件目录、链接关系
5. 最后再接触测试、安装、第三方依赖这些进阶内容

----

# 开发过程中的注意点

## 1. 构建脚本也是项目源码的一部分

很多初学者只重视 `.cpp/.h`，却把 `Makefile`、`CMakeLists.txt` 当成“附属配置”。  
实际上它们决定了项目能不能被别人正确构建出来。

所以构建脚本也应当：

- 可读
- 有结构
- 命名清晰
- 尽量少写魔法逻辑

## 2. 不要把所有配置写成全局

尤其在 `CMake` 中，现代实践更推荐：

- 使用 target 级别的配置
- 少用全局 `include_directories()`
- 少用全局 `link_libraries()`

因为 target 级配置更清楚地表达了“谁依赖谁”。

## 3. 头文件路径和链接库是两回事

很多人第一次接第三方库时会犯这个错：

- 头文件能找到，就以为万事大吉

其实还要确认：

- 是否正确链接库
- 动态库运行时是否能被找到

## 4. Debug 和 Release 要分开看

不同构建类型可能意味着：

- 优化等级不同
- 调试信息不同
- 宏定义不同
- 运行结果不同

所以不要拿 Debug 通过就直接等价成 Release 没问题。

## 5. 构建目录坏了就敢于重建

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

## 6. 先让流程简单可解释，再追求高级技巧

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

# 一条最推荐的入门路线

如果你是零基础，建议按这个顺序理解：

## 第一步：手工编译一个程序

先亲手写过：

```bash
g++ main.cpp -o app
```

知道“编译器到底在干什么”。

## 第二步：手工编译多个文件

例如：

```bash
g++ main.cpp math.cpp -o app
```

理解“项目不是只有一个文件”。

## 第三步：理解 `make`

至少看懂下面三件事：

- 目标
- 依赖
- 规则

理解“为什么改一个头文件会触发某些文件重编译”。

## 第四步：理解 `cmake`

知道这些基本命令：

```bash
cmake -S . -B build
cmake --build build
```

知道 `CMakeLists.txt` 至少会写：

- `project()`
- `add_executable()`
- `add_library()`
- `target_include_directories()`
- `target_link_libraries()`

## 第五步：再进入真实工程能力

例如：

- 第三方库管理
- 多目录项目
- 测试集成
- 安装与打包
- CI 自动构建

这样学习会稳很多。

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
