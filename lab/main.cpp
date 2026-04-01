#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>

class Buffer {
public:
    Buffer(std::string name, std::size_t size)
        : name_(std::move(name)), size_(size), data_(new int[size]) {
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = static_cast<int>(i + 1);
        }

        std::cout << "构造: " << name_
                  << " 持有地址 " << static_cast<void*>(data_) << '\n';
    }

    ~Buffer() {
        std::cout << "析构: " << name_
                  << " 释放地址 " << static_cast<void*>(data_) << '\n';
        delete[] data_;
    }

    Buffer(const Buffer& other)
        : name_(other.name_ + "_copy"),
          size_(other.size_),
          data_(other.size_ == 0 ? nullptr : new int[other.size_]) {
        if (size_ > 0) {
            std::copy(other.data_, other.data_ + size_, data_);
        }
        std::cout << "拷贝构造: 从 " << other.name_
                  << " 深拷贝出 " << name_
                  << "，新地址是 " << static_cast<void*>(data_) << '\n';
    }

    Buffer& operator=(const Buffer& other) {
        if (this == &other) {
            return *this;
        }

        int* new_data = other.size_ == 0 ? nullptr : new int[other.size_];
        if (other.size_ > 0) {
            std::copy(other.data_, other.data_ + other.size_, new_data);
        }

        delete[] data_;
        data_ = new_data;
        size_ = other.size_;
        name_ = other.name_ + "_copy_assign";
        std::cout << "拷贝赋值: 现在 " << name_
                  << " 拥有独立地址 " << static_cast<void*>(data_) << '\n';
        return *this;
    }

    Buffer(Buffer&& other) noexcept
        : name_(std::move(other.name_)),
          size_(other.size_),
          data_(other.data_) {
        std::cout << "移动构造: " << name_
                  << " 接管地址 " << static_cast<void*>(data_) << '\n';
        other.name_ = "[已搬空]";
        other.size_ = 0;
        other.data_ = nullptr;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        delete[] data_;
        name_ = std::move(other.name_);
        data_ = other.data_;
        size_ = other.size_;
        std::cout << "移动赋值: " << name_
                  << " 接管地址 " << static_cast<void*>(data_) << '\n';

        other.name_ = "[已搬空]";
        other.data_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    void set(std::size_t index, int value) {
        if (data_ != nullptr && index < size_) {
            data_[index] = value;
        }
    }

    const int* raw_data() const {
        return data_;
    }

    void print() const {
        std::cout << name_ << " | data=" << static_cast<const void*>(data_) << " | values=";
        if (data_ == nullptr) {
            std::cout << "<null>\n";
            return;
        }

        for (std::size_t i = 0; i < size_; ++i) {
            std::cout << data_[i] << ' ';
        }
        std::cout << '\n';
    }

private:
    std::string name_;
    std::size_t size_ = 0;
    int* data_ = nullptr;
};

int main() {
    Buffer a("a", 3);
    a.print();

    std::cout << "---- 拷贝构造 ----\n";
    Buffer b = a;
    b.set(0, 99);
    a.print();
    b.print();
    std::cout << "a 和 b 是否共享同一块内存: "
              << std::boolalpha << (a.raw_data() == b.raw_data()) << '\n';

    std::cout << "---- 移动构造 ----\n";
    Buffer c = std::move(a);
    a.print();
    c.print();
    std::cout << "a 移动后是否为空: "
              << std::boolalpha << (a.raw_data() == nullptr) << '\n';
}
