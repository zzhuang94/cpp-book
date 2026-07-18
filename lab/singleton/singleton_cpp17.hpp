// C++17 单例模板，利用 inline static 简化实现
template <typename T>
struct singleton2 {
    singleton2(const singleton2&) = delete;
    singleton2& operator=(const singleton2&) = delete;
    singleton2(singleton2&&) = delete;
    singleton2& operator=(singleton2&&) = delete;

    static T& get_instance() {
        // 局部静态变量，保证只构造一次（C++11 起线程安全）
        static T obj;
        return obj;
    }

private:
    singleton2() = default;
};
