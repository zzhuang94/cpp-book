#include <iostream>
#include <string>

struct Book {
    std::string title;
    int pages;

    void print() const {
        std::cout << title << ", " << pages << std::endl;
    }
};

int main() {
    Book b1;
    Book b2{"C++11", 500};

    b1.print();
    b2.print();
    return 0;
}
