# I/O

`C++` 的输入输出主要基于流（stream）模型。
你以前可能已经写过 `cin`、`cout`、文件流，但在 `C++11` 项目里：
> I/O 的重点通常不只是“怎么读写”，还包括“如何与字符串、文件和格式化逻辑组合使用”。

----

# 标准输出

```cpp
#include <iostream>

int main() {
    std::cout << "Hello C++11" << std::endl;
    return 0;
}
```

常见流对象：

- `std::cin`：标准输入
- `std::cout`：标准输出
- `std::cerr`：标准错误输出
- `std::clog`：日志类输出

----

# 标准输入

```cpp
#include <iostream>

int main() {
    int age = 0;
    std::cin >> age;
    std::cout << "age = " << age << std::endl;
    return 0;
}
```

`>>` 会按空白符切分输入，对于“包含空格的整行文本”不合适。

这里可以先建立一个直觉：流不是“几个长得像函数的符号”，而是带状态的对象。你对它做一次读取、写入或打开文件操作，都会改变这个对象后续的状态。

----

# 读取整行

```cpp
#include <iostream>
#include <string>

int main() {
    std::string line;
    std::getline(std::cin, line);
    std::cout << line << std::endl;
    return 0;
}
```

如果前面已经用了 `>>`，再接 `getline()` 时要小心遗留换行符。

例如下面这种组合就很常见：

```cpp
#include <iostream>
#include <limits>
#include <string>

int main() {
    int age = 0;
    std::string name;

    std::cin >> age;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, name);

    std::cout << age << ", " << name << std::endl;
    return 0;
}
```

如果不先处理掉缓冲区里的换行符，`getline()` 很可能直接读到一个空串。

----

# 文件 I/O

## 写文件

```cpp
#include <fstream>

int main() {
    std::ofstream out("demo.txt");
    out << "hello file" << std::endl;
    return 0;
}
```

文件流和前面的标准输入输出流一样，也是对象。它的一个典型好处是：
> 对象离开作用域时，会自动关闭文件，这正是 `C++` 常说的 RAII 风格。

## 读文件

```cpp
#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::ifstream in("demo.txt");
    std::string line;

    while (std::getline(in, line)) {
        std::cout << line << std::endl;
    }

    return 0;
}
```

读取文件时不要把“成功打开”和“成功读到内容”混成一件事。更稳妥的思路通常是：

- 先检查文件是否打开成功
- 再进入读取循环
- 每次读取都让流状态自己决定是否继续

----

# 字符串流

字符串流非常适合做轻量解析或拼装文本：

```cpp
#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::stringstream ss;
    ss << "name=" << "cpp11" << ", version=" << 11;

    std::string text = ss.str();
    std::cout << text << std::endl;
    return 0;
}
```

也可以用于解析：

```cpp
#include <iostream>
#include <sstream>

int main() {
    std::stringstream ss("100 3.14");
    int a;
    double b;
    ss >> a >> b;

    std::cout << a << ", " << b << std::endl;
    return 0;
}
```

你可以把 `stringstream` 暂时理解成一个“内存里的小型文本缓冲区”：

- 往里写时，它像输出流
- 从里读时，它像输入流
- 当需要在“文本”和“结构化字段”之间来回转换时，它很方便

----

# 状态检查

流对象支持状态判断：

```cpp
if (!in) {
    // 打开文件失败
}
```

或者：

```cpp
while (std::getline(in, line)) {
}
```

这类写法本质上是在利用流对象的状态转换成布尔值。

这也是为什么很多 `C++` I/O 代码读起来不像“先调用函数拿结果”，而更像“不断操作同一个有状态对象”。

----

# 综合示例

```cpp
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::ofstream out("numbers.txt");
    out << "10 20 30" << std::endl;
    out.close();

    std::ifstream in("numbers.txt");
    std::string line;

    if (std::getline(in, line)) {
        std::stringstream ss(line);
        int x, y, z;
        ss >> x >> y >> z;
        std::cout << (x + y + z) << std::endl;
    }

    return 0;
}
```

----

# 推荐实践

- 命令行交互优先使用 `cin/cout`。
- 读取整行文本优先使用 `getline()`。
- 轻量文本解析优先使用 `stringstream`。
- 文件操作后检查流状态，不要假设文件一定存在。
- 需要日志或错误输出时区分 `cout` 与 `cerr`。

# 小结

这章真正要建立的判断，是“我现在是在和哪个流对象打交道，它的状态是什么，以及这次读取的边界到底在哪里”。

- `>>` 更适合按字段读取
- `getline()` 更适合按整行读取
- 文件流和字符串流常常是标准输入输出之外的两个重要补充

把流看成“可以组合、会变化状态的对象”之后，`C++` 的 I/O 写法就不会只剩一堆零散语法了。
