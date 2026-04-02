#include <iostream>

class User {
public:
  explicit User(int id) : id_(id) {}

private:
  int id_;
};
int main() {
  // User user;   // 错误：没有默认构造函数
  User user(1);
  return 0;
}