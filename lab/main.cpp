#include <iostream>

class Base {
public:
    Base(std::string name) : name_(std::move(name)) {
        std::cout << "构造基类, name: " << name_ << std::endl; 
    }
    ~Base() { std::cout << "销毁基类, name: " << name_ << std::endl; }
public:
    std::string name_;
};

class Derived : public Base {
public:
    Derived(std::string name) : Base(std::move(name)) {
        std::cout << "构造派生类, name: " << name_ << std::endl; 
    }
    ~Derived() { std::cout << "销毁派生类, name: " << name_ << std::endl; }
};

int main() {
    Derived a("AAA");
    Derived b("BBB");
    return 0;
}