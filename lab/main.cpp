#include <iostream>

void print() {
    std::cout << std::endl;
}

template <typename T, typename... Args>
void print(const T& first, const Args&... rest) {
    std::cout << first << " ";
    print(rest...);
}

int main() {
    print(1, "hello", 3.14);
    return 0;
}