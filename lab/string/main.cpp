#include <iostream>
#include <string>

void test(const char* str) {
    std::cout << "str: " << str << std::endl;
}

void test2(const std::string& str) {
    std::cout << "str: " << str << std::endl;
}

int main() {
    const char* str = "hello";
    test(str);
    test2(str);

    const std::string s = "hello";
    test(s.c_str());
    test2(s);
    return 0;
}