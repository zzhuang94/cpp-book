#include <iostream>

void f(int) {
    std::cout << "int" << std::endl;
}

void f(int*) {
    std::cout << "pointer" << std::endl;
}

int main() {
    f(nullptr);
    f(0);
    // f(NULL); 编译失败 call of overloaded 'f(NULL)' is ambiguous
    return 0;
}
