#include <iostream>

template <typename Func>
void repeat(int n, Func func) {
    for (int i = 0; i < n; ++i) {
        func("haha", i);
    }
}
int main() {
    repeat(3, [](std::string name, int i) {
        std::cout << name << " " << i << std::endl;
    });
    return 0;
}
