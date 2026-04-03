#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

std::string readFirstLine(const std::string& fileName) {
    std::ifstream input(fileName.c_str());
    if (!input) {
        throw std::runtime_error("failed to open file");
    }
    std::string line;
    if (!std::getline(input, line)) {
        throw std::runtime_error("failed to read line");
    }
    return line;
}

int main() {
    try {
        std::cout << readFirstLine("demo.txt") << std::endl;
    } catch (const std::exception& ex) {
        std::cout << "error: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
