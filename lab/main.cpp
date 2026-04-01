#include <iostream>

void print(int value) {
    std::cout << "普通函数版本，处理 int: " << value << std::endl;
}

template <typename T>
void print(T value) {
    std::cout << "函数模板版本，处理其他类型: " << value << std::endl;
}

int main() {
    print(10);
    print(3.14);

    return 0;
}
