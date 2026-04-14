#include <functional>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

struct Task {
    std::string name;
    int priority;
};

struct TaskCompare {
    bool operator()(const Task& a, const Task& b) const {
        if (a.priority < b.priority) {
            return true;
        }
        if (a.priority > b.priority) {
            return false;
        }
        return false;
    }
};

int main() {
    std::priority_queue<int> big_top;
    big_top.push(3);
    big_top.push(1);
    big_top.push(4);
    std::cout << "max-heap pop:";
    while (!big_top.empty()) {
        int v = big_top.top();
        big_top.pop();
        std::cout << ' ' << v;
    }
    std::cout << "\n";

    std::priority_queue<int, std::vector<int>, std::greater<int> > small_top;
    small_top.push(3);
    small_top.push(1);
    small_top.push(4);
    std::cout << "min-heap pop:";
    while (!small_top.empty()) {
        int v = small_top.top();
        small_top.pop();
        std::cout << ' ' << v;
    }
    std::cout << "\n";

    std::priority_queue<Task, std::vector<Task>, TaskCompare> jobs;
    jobs.push(Task{"log", 1});
    jobs.push(Task{"fire", 9});
    jobs.push(Task{"mail", 3});

    std::cout << "jobs by priority:";
    while (!jobs.empty()) {
        Task t = jobs.top();
        jobs.pop();
        std::cout << ' ' << t.name << '(' << t.priority << ')';
    }
    std::cout << "\n";

    return 0;
}
