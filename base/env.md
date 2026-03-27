# 环境准备

开始重新学习 `C++` 时，第一件事不是背语法，而是确保你的编译环境真的在用 `C++11`。很多“代码为什么不能编译”的问题，本质上只是编译器标准版本不对。

## 推荐编译器

常见选择：

- `g++`，适合 Linux、WSL、MinGW 环境
- `clang++`，错误信息友好，诊断体验通常更好
- `MSVC`，Windows 项目中非常常见

如果你要接手现有项目，优先确认：

- 项目使用哪套编译器
- 是否显式开启了 `C++11`
- 构建系统是 `Makefile`、`CMake` 还是 IDE 工程

## 开启 C++11 标准

对于 `g++` 或 `clang++`，最常见的命令是：

```bash
g++ -std=c++11 main.cpp -o main
```

如果不写 `-std=c++11`，编译器可能会：

- 默认启用较老标准
- 或启用更高标准，但行为和项目配置不一致

接手项目时，最好和项目配置保持一致，不要一开始就私自升级到 `C++17` 或 `C++20`。

## 第一个验证程序

用下面这段代码验证你的环境是否支持 `C++11`：

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> data{10, 20, 30};

    for (auto item : data) {
        std::cout << item << std::endl;
    }

    return 0;
}
```

如果下面这些语法都能编译通过，基本说明 `C++11` 已经正常启用：

- 列表初始化 `{10, 20, 30}`
- `auto`
- 范围 `for`

## 推荐的目录方式

学习阶段建议按“一个知识点一个文件”的方式组织示例：

```text
demo/
  auto_demo.cpp
  lambda_demo.cpp
  move_demo.cpp
```

这样做的好处是：

- 每个示例都能独立编译
- 容易定位错误
- 便于对比不同语法点

## 常用编译参数

除了 `-std=c++11`，还推荐在学习阶段开启更多警告：

```bash
g++ -std=c++11 -Wall -Wextra -pedantic main.cpp -o main
```

这些参数有助于你尽早发现：

- 隐式类型转换问题
- 未使用变量
- 可疑的控制流
- 旧式写法带来的潜在风险

## 一个简单的 Makefile 例子

如果你希望多次编译更方便，可以准备一个最小 `Makefile`：

```makefile
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic

all: demo

demo: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o demo
```

## CMake 中启用 C++11

很多现代 `C++` 项目都使用 `CMake`。最常见的写法如下：

```cmake
cmake_minimum_required(VERSION 3.5)
project(cpp11_demo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cpp11_demo main.cpp)
```

## 版本检查示例

你可以用宏 `__cplusplus` 粗略判断编译标准：

```cpp
#include <iostream>

int main() {
    std::cout << __cplusplus << std::endl;
    return 0;
}
```

在支持 `C++11` 的环境里，这个值通常至少是 `201103L`。

## 学习阶段的建议

- 先使用命令行编译，理解“源文件 -> 编译 -> 链接”的过程。
- 每个示例都亲手编译一次，不要只看不跑。
- 一旦示例涉及资源管理，优先尝试智能指针和标准库容器。
- 养成看编译警告的习惯，不要只盯着 error。

## 小结

环境准备的核心不是“装好编译器”这么简单，而是明确你正在使用的标准、编译参数和构建方式。只有环境稳定了，后续学习 `auto`、`lambda`、移动语义这些 `C++11` 特性时，结论才不会被工具链噪音干扰。
