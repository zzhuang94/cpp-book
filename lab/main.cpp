#include <iostream>
#include <string>
#include <utility>

void print(const std::string& text) {
    std::cout << "const ref: " << text << std::endl;
}

void print(std::string&& text) {
    std::cout << "right value ref: " << text << std::endl;
}

void printMove(std::string text) {
    std::string tmp = std::move(text);
    std::cout << "moved value: " << tmp << std::endl;
}

int main() {
    print("temporary");
    
    std::string name = "cpp11";

    print(name);
    std::cout << "name1 = " << name << std::endl;

    print(std::move(name));
    std::cout << "name2 = " << name << std::endl;

    printMove(name);
    std::cout << "name3 = " << name << std::endl;
    
    printMove(std::move(name));
    std::cout << "name4 = " << name << std::endl;
    return 0;
}
