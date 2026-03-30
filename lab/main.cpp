#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> nums{3, 1, 2, 4};
    auto it = std::find(nums.begin(), nums.end(), 2);

    if (it != nums.end()) {
        std::cout << *it << std::endl;
        *it = 20;
    }
    for (auto n : nums) {
        std::cout << n << " ";
    }
    std::cout << std::endl;
    return 0;
}
