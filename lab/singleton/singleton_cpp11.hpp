// C++11 单例模板（Meyers Singleton）
template <typename T>
class singleton {
public:
    static T& get_instance() {
        // 局部静态变量，C++11 保证线程安全的一次性初始化
        static T obj;
        return obj;
    }

    // 禁止拷贝
    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;

    // 禁止移动
    singleton(singleton&&) = delete;
    singleton& operator=(singleton&&) = delete;

private:
    singleton() = default;
    ~singleton() = default;
};
