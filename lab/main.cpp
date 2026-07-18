#include <iostream>
#include <vector>

// 1. 类模板声明
template <typename T>
class Box {
public:
    static int count; // 静态成员变量声明

    Box() {
        count++; // 每次创建对象时，对应的 count 加 1
    }
};

auto func = []() {
    std::cout << "init count" << std::endl;
    return 0;
};

// 2. 静态成员变量的类外定义和初始化
template <typename T>
int Box<T>::count = func(); 

class Fox {
public:
    static int count;

    Fox() {
        count++;
    }
    
    std::vector<int> nums_;
};

int Fox::count = 0;

int main() {
    std::cout << "######## main start ########" << std::endl;
    Box<int> b1;
    Box<int> b2;
    
    Box<double> d1;

    // Box<int> 和 Box<double> 拥有各自独立的 count
    std::cout << "Box<int> count: " << Box<int>::count << std::endl;       // 输出 2
    std::cout << "Box<double> count: " << Box<double>::count << std::endl; // 输出 1

    Fox f1;
    Fox f2;
    std::cout << "Fox count: " << Fox::count << std::endl;
    std::cout << "Fox nums size: " << f1.nums_.size() << std::endl;

    return 0;
}