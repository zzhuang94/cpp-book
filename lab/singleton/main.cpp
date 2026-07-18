// 引入单例模板头文件
#include "singleton.hpp"

#include <iostream>

// 一个普通的测试类，构造时会打印信息
class Test {
public:
    Test() {
        std::cout << "Test constructor" << std::endl;
    }
};

int main() {
    std::cout << "######## main start ########" << std::endl;
    // 第一次获取 Test 的单例引用，此时会触发 Test 的构造函数
    Test& test = singleton2<Test>::get_instance();
    std::cout << "test address: " << &test << std::endl;
    std::cout << "--------------------------------" << std::endl;

    // 第二次获取单例引用，不会再构造新对象，返回同一个实例
    Test& test2 = singleton2<Test>::get_instance();
    std::cout << "test2 address: " << &test2 << std::endl;

    // test 和 test2 的地址相同，证明是单例
    return 0;
}
