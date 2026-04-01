# std::copy

在 C++11 里，`std::copy` 是最常见、也最值得尽早掌握的标准算法之一。  
它的作用很直接：**把一段范围里的元素，按顺序复制到另一段目标范围中**。

很多初学者第一次接触它，通常是在下面这些场景：

- 把数组内容复制到另一个数组
- 把一个 `std::vector` 的元素复制到另一个容器
- 把数据追加到目标容器末尾
- 配合迭代器统一处理“原生数组”和“标准库容器”

如果只记一句话，可以记成：

> `std::copy` 用来把区间 `[first, last)` 中的元素，顺序复制到从 `d_first` 开始的目标位置。

---

# 背景

为什么 C++ 标准库专门提供 `std::copy`，而不是让我们一直手写 `for` 循环？

因为“复制一段数据”是非常高频的基础操作，而标准算法的意义就在于：

- 用统一接口表达常见操作
- 让代码更容易读出“意图”
- 让数组、容器、迭代器都能用同一种写法处理

在 C++11 里，这种“算法 + 迭代器”的写法尤其重要。  
你会越来越多地看到这种风格：

```cpp
std::copy(begin, end, out);
```

它不关心底层到底是数组、`std::vector`、`std::list`，还是别的容器；  
只要提供合适的迭代器，算法就能工作。

这也是学习标准库时一个很关键的转变：

- 不再只想着“我怎么写循环”
- 而是先想“标准库有没有现成算法表达这个动作”

---

# 基本形式

`std::copy` 定义在头文件 `<algorithm>` 中，最常见的形式可以理解为：

```cpp
#include <algorithm>

copy(first, last, d_first);
```

其中：

- `first` 表示源区间起点
- `last` 表示源区间终点的后一位
- `d_first` 表示目标区间起点

也就是说，它会把：

```cpp
[first, last)
```

中的每个元素，依次复制到目标位置。

返回值是复制结束后，目标区间“下一位”的迭代器。  
大多数入门场景下你可以先不依赖这个返回值，但理解它会有助于以后做更灵活的链式操作。

---

# 常见使用场景

## 1. 数组复制到数组

这是最直观的入门场景：

```cpp
#include <algorithm>
#include <iostream>

int main() {
    int src[5] = {1, 2, 3, 4, 5};
    int dst[5] = {};

    std::copy(src, src + 5, dst);

    for (int x : dst) {
        std::cout << x << " ";
    }
}
```

这里：

- `src` 是起点
- `src + 5` 是终点后一位
- `dst` 是目标起点

这种写法很适合先帮助理解 `std::copy` 最原始的工作方式。

## 2. 容器复制到另一个已分配好空间的容器

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> a{10, 20, 30};
    std::vector<int> b(a.size());

    std::copy(a.begin(), a.end(), b.begin());

    for (int x : b) {
        std::cout << x << " ";
    }
}
```

这是 `std::copy` 最常见的标准写法之一。  
注意这里的关键前提是：**`b` 已经有足够空间容纳复制结果**。

如果目标容器只是“空的”，但没有实际元素位置可写，直接 `b.begin()` 并不能解决问题，这一点后面会专门说。

## 3. 追加到目标容器末尾

如果目标容器还没有足够元素，通常不应该直接往 `begin()` 写，而是配合插入迭代器：

```cpp
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

int main() {
    std::vector<int> a{1, 2, 3};
    std::vector<int> b{100, 200};

    std::copy(a.begin(), a.end(), std::back_inserter(b));

    for (int x : b) {
        std::cout << x << " ";
    }
}
```

输出结果相当于把 `a` 的内容追加到了 `b` 后面。

在 C++11 日常代码里，`std::copy + std::back_inserter(...)` 非常常见，  
因为它比“先手动扩容、再复制”更自然，也更不容易写错。

## 4. 从原生数组复制到 `std::vector`

```cpp
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

int main() {
    int arr[] = {7, 8, 9};
    std::vector<int> nums;

    std::copy(arr, arr + 3, std::back_inserter(nums));

    for (int x : nums) {
        std::cout << x << " ";
    }
}
```

这体现了算法 + 迭代器风格的价值：  
源数据是原生数组，目标是标准库容器，但复制方式依然统一。

## 5. 输出到流

`std::copy` 的目标不一定非得是另一个容器，也可以是输出流迭代器：

```cpp
#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

int main() {
    std::vector<int> nums{1, 2, 3, 4};

    std::copy(nums.begin(), nums.end(),
              std::ostream_iterator<int>(std::cout, " "));
}
```

这说明 `std::copy` 复制的本质不是“拷到数组里”，而是：

- 从一个输入范围读元素
- 向一个输出位置依次写元素

只要迭代器满足要求，目标可以很灵活。

---

# 返回值怎么理解

`std::copy` 的返回值是“目标区间中最后一个被复制元素的下一位”。

例如：

```cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> a{1, 2, 3};
    std::vector<int> b(5, 0);

    std::vector<int>::iterator it = std::copy(a.begin(), a.end(), b.begin());

    *it = 99;

    for (int x : b) {
        std::cout << x << " ";
    }
}
```

执行后，`b` 的前 3 个位置被复制，`it` 会指向第 4 个位置。  
这类写法在“先复制一段，再接着写别的内容”时会比较方便。

---

# 注意事项

`std::copy` 看起来很简单，但真正容易出错的地方主要集中在目标区间和区间重叠上。

## 1. 目标区间必须足够大

下面这种写法是错误的高发区：

```cpp
std::vector<int> a{1, 2, 3};
std::vector<int> b;

std::copy(a.begin(), a.end(), b.begin());  // 错误
```

问题不在于 `b` 是不是 `vector`，而在于 `b` 此时还没有可写入的现有元素。  
`b.begin()` 并不代表“自动追加”。

更安全的方式通常是二选一：

```cpp
std::vector<int> b(a.size());
std::copy(a.begin(), a.end(), b.begin());
```

或者：

```cpp
std::vector<int> b;
std::copy(a.begin(), a.end(), std::back_inserter(b));
```

可以简单记成：

- 写到已有位置，目标容器必须先有空间
- 想自动追加，使用 `std::back_inserter`

## 2. 不要把它当成“自动扩容复制”

很多人第一次看到 `std::copy`，会下意识把它想成“把 a 复制到 b”。  
但它真正做的不是容器层面的复制，而是**通过输出迭代器逐个写元素**。

所以它不会主动帮你：

- `resize`
- `push_back`
- 检查容量是否足够

除非你显式使用了像 `std::back_inserter` 这样的插入迭代器。

## 3. 源区间和目标区间重叠时要小心

如果复制范围发生重叠，`std::copy` 不能保证结果符合你的直觉。

比如在同一个容器里移动一段数据位置时，写法就要特别谨慎。  
这类场景更常见的安全选择是 `std::copy_backward`。

可以先记住这个经验：

- 普通不重叠复制，用 `std::copy`
- 从后往前处理重叠区域，考虑 `std::copy_backward`

## 4. 它执行的是“复制赋值”，不是“移动”

在 C++11 里我们已经有了移动语义，但 `std::copy` 依然表达的是“复制”。

也就是说，它通常要求目标元素可以接收复制赋值或复制构造。  
如果你的意图是把资源“搬走”而不是“复制一份”，那应该进一步了解移动相关工具，而不是继续用 `std::copy` 硬套。

对于初学阶段，可以先建立这个区分：

- `std::copy` 语义是复制
- “移动语义”是另一类问题，不要混为一谈

## 5. 头文件别忘了是 `<algorithm>`

这是一个很小但很常见的细节。  
`std::copy` 定义在：

```cpp
#include <algorithm>
```

如果还用到了 `std::back_inserter` 或 `std::ostream_iterator`，通常还需要：

```cpp
#include <iterator>
```

---

# 和直接写循环对比

下面这两段代码，本质上是在表达同一件事。

手写循环：

```cpp
for (std::size_t i = 0; i < a.size(); ++i) {
    b[i] = a[i];
}
```

使用 `std::copy`：

```cpp
std::copy(a.begin(), a.end(), b.begin());
```

对比来看：

- 手写循环更容易暴露下标细节
- `std::copy` 更直接表达“复制这个范围”的意图
- 当代码进入“算法 + 迭代器”的统一风格后，可读性通常更好

当然，前提是你已经能熟悉 `[first, last)` 和迭代器这套表示方式。

---

# 什么时候适合优先想到它

在 C++11 日常代码里，看到下面这些需求时，可以优先想一想 `std::copy`：

- 把一个范围的数据原样复制到另一个地方
- 不想显式写下标循环
- 希望统一处理数组和容器
- 希望配合插入迭代器把数据追加到目标容器

如果你的需求已经不是“原样复制”，而是：

- 复制时顺便变换元素
- 按条件筛选再写入
- 需要移动而不是复制

那往往就该考虑别的算法，而不是继续死守 `std::copy`。

---

# 小结

在 C++11 背景下，`std::copy` 最重要的不是记住模板长什么样，而是先理解它的使用模型：

- 它复制的是区间 `[first, last)`
- 它通过目标迭代器把元素依次写出去
- 目标区间必须可写，而且空间要足够
- 目标如果需要自动追加，常配合 `std::back_inserter`
- 遇到重叠区间或“想移动不想复制”的场景，要特别小心

把它理解成“标准库提供的范围复制算法”，后面再学其他算法时会顺很多。  
因为很多标准算法，本质上都是同一种思路：**用迭代器描述范围，用算法表达意图**。