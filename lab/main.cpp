#include <iostream>

class Test {
public:
    Test() { std::cout << "construct" << std::endl; }
    ~Test() { std::cout << "destruct" << std::endl; }
};

int main() {
    Test* p = new Test;
    delete p;
}
