#include <iostream>
#include <vector>
#include <ostream>
#include <string_view>
#include <set>
#include <deque>

#include "mem_pool_alloc.hpp"

using namespace memory;

template<typename C, typename A>
std::ostream& operator<<(std::ostream& os, const std::vector<C, A>& rhs) {
    if (rhs.empty()) {
        return os << "[]";
    }
    os << typeid(C).name() << '[';
    for (auto it = rhs.cbegin(); it != rhs.cend() - 1; ++it) {
        os << *it << ", ";
    }
    os << rhs.back() << ']';

    return os;
}

int main() {

    //std::vector<int, pool_allocator<int>> vector(pool_allocator<int>(16, 1024));
    std::set<float, std::greater<>, pool_allocator<float>> set(pool_allocator<float>(16, 10));
    //std::deque<char, pool_allocator<char>> deque(pool_allocator<char>(16, 1024));

    for (int i = 0; i < 59; ++i)
        set.insert(i*3.14);

    for (int i = 0; i < 3; ++i)
        set.extract(i * 3.14);
}
