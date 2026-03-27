#include <iostream>
#include "math.h"

extern int add(int a, int b);

int main() {
    std::cout << add(2, 3) << std::endl;
    return 0;
}