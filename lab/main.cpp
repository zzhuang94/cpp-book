#include <iostream>
#include <memory>
#include <vector>

class Printer {
public:
    virtual ~Printer() = default;
    virtual void print(int value) const = 0;
};

class ConsolePrinter : public Printer {
public:
    void print(int value) const override {
        std::cout << value << std::endl;
    }
};

int main() {
    std::vector<int> values{1, 2, 3};
    std::unique_ptr<Printer> printer(new ConsolePrinter());

    for (const auto& value : values) {
        printer->print(value);
    }

    return 0;
}