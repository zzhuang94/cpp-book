// C++98 单例模板
template <typename T>
class singleton {
public:
    static T& get_instance() {
        // C++98 中局部 static 不保证线程安全，
        // 单线程环境下可以这样用；多线程需要额外加锁
        static T obj;
        return obj;
    }

private:
    // 构造和析构私有，禁止外部创建实例
    singleton();
    ~singleton();

    // C++98 没有 = delete，通过"声明但不定义"来禁止拷贝：
    // 如果有人试图拷贝，链接阶段会报 undefined reference 错误
    singleton(const singleton&);
    singleton& operator=(const singleton&);
};
