#include <iostream>

class Test {
public:
    Test() {
        std::cout << "Test constructor" << std::endl;
    }
    ~Test() {
        std::cout << "Test destructor" << std::endl;
        delete p;
    }
    void print() {
        std::cout << "p: " << p << std::endl;
    }
private:
    int* p;
};

void test() {
    Test* t = new Test;
    t->print();
    delete t;
    if (t != nullptr) {
        delete t;
    }
    std::cout << "test end" << std::endl;
}

int main() {
    test();
    return 0;
}

