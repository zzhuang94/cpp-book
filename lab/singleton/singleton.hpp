#include <iostream>

// 模板单例类，T 为需要成为单例的类型
template <typename T>
struct singleton2 {
private:
	// 内部辅助结构体，用于在 main() 之前触发单例实例的创建
	struct EarlyInitializer {
		// 构造函数中调用 get_instance()，确保单例对象在程序启动阶段就被初始化
		EarlyInitializer() { 
            std::cout << "EarlyInitializer constructor" << std::endl;
            singleton2<T>::get_instance(); 
            std::cout << "EarlyInitializer constructor end" << std::endl;
        }
		// 空函数，仅用于在 get_instance() 中"使用" early_init_，
		// 迫使编译器实例化该静态成员，从而触发其构造函数
		inline void do_nothing() const {};
	};
	// 静态成员，程序启动时（main 之前）被构造，从而提前初始化单例
	static EarlyInitializer early_init_;

public:
	typedef T value_type;
	// 获取单例实例的核心方法
	static value_type & get_instance() {
		std::cout << "get_instance start" << std::endl;
		// 局部静态变量，保证只构造一次（C++11 起线程安全）
		static value_type obj;
		// 调用 do_nothing() 确保 early_init_ 被实例化（ODR-use），
		// 否则编译器可能优化掉 early_init_，导致提前初始化失效                                                                                                                                                                                                                               
		early_init_.do_nothing();
		std::cout << "get_instance end" << std::endl;
		return obj;
	}
};

// 在类外定义静态成员 early_init_，触发其默认构造
template <typename T>
typename singleton2<T>::EarlyInitializer singleton2<T>::early_init_;
