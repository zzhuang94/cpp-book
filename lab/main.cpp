#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

int main() {
    std::map<std::string, int> score;

    // operator[]：写入或读已存在键
    score["Alice"] = 90;
    score["Bob"] = 85;

    if (score.find("Carol") == score.end()) {
        std::cout << "Carol not in table (find does not insert)\n";
    }

    // insert：已存在则不覆盖
    auto ins1 = score.insert(std::make_pair("Bob", 0));
    if (!ins1.second) {
        std::cout << "Bob already there, value still " << ins1.first->second << "\n";
    }

    score.emplace("Dave", 88);

    // at：键必须存在
    try {
        int a = score.at("Alice");
        std::cout << "Alice score=" << a << "\n";
        (void)score.at("Nobody");
    } catch (const std::out_of_range& ex) {
        std::cout << "at() missing key: " << ex.what() << "\n";
    }

    // 遍历：按键序
    for (auto it = score.begin(); it != score.end(); ++it) {
        std::cout << it->first << " => " << it->second << "\n";
    }

    // lower_bound / upper_bound
    std::map<int, char> rank;
    rank[10] = 'a';
    rank[20] = 'b';
    rank[30] = 'c';

    std::map<int, char>::iterator lo = rank.lower_bound(15);
    std::map<int, char>::iterator hi = rank.upper_bound(25);
    std::cout << "keys in (15,25] by iterators:\n";
    for (std::map<int, char>::iterator j = lo; j != hi; ++j) {
        std::cout << "  " << j->first << " -> " << j->second << "\n";
    }

    // 删除
    if (score.erase("Bob") > 0) {
        std::cout << "Bob erased\n";
    }

    return 0;
}
