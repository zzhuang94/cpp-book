# I/O

`C++` 的输入输出主要基于流（stream）模型。你以前可能已经写过 `cin`、`cout`、文件流，但在 `C++11` 项目里，I/O 的重点通常不只是“怎么读写”，还包括“如何与字符串、文件和格式化逻辑组合使用”。

## 标准输出

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

## 标准输入

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

## 读取整行

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

## 文件输入输出

### 写文件

```cpp
#include <fstream>

int main() {
    std::ofstream out("demo.txt");
    out << "hello file" << std::endl;
    return 0;
}
```

### 读文件

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

## 字符串流

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

## 流状态检查

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

## 一个综合示例

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

## 推荐实践

- 命令行交互优先使用 `cin/cout`。
- 读取整行文本优先使用 `getline()`。
- 轻量文本解析优先使用 `stringstream`。
- 文件操作后检查流状态，不要假设文件一定存在。
- 需要日志或错误输出时区分 `cout` 与 `cerr`。

## 小结

`C++` 的 I/O 体系本身并没有因为 `C++11` 发生根本变化，但现代写法更强调“流对象之间的组合能力”。当你把标准输入输出、文件流和字符串流串起来用时，很多文本处理任务会比手写缓冲区简单得多。
