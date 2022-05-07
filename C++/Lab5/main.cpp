#include <iostream>
#include <vector>
#include <ostream>
#include <random>
#include <iomanip>
#include <string_view>
#include <set>
#include <deque>
#include <chrono>

#include "../Lab3/circular.hpp"
#include "mem_pool_alloc.hpp"

class timer {
    private:
        std::string _msg;
        std::chrono::time_point<std::chrono::steady_clock> _start;
    public:
        timer(std::string_view msg) : _msg(msg), _start(std::chrono::steady_clock::now()) {}
        timer() : timer("") {}

        ~timer() {
            using namespace std::chrono;
            auto duration = duration_cast<std::chrono::duration<double, std::milli>>(steady_clock::now() - _start);
            std::cout << (_msg.empty() ? "" : _msg + ": ")
                      << std::setprecision(4)
                      << duration.count() << "ms elapsed" << std::endl;
        }
};


using namespace memory;

template<typename C, typename A>
std::ostream& operator<<(std::ostream& os, const std::vector<C, A>& rhs) {
    if (rhs.empty()) {
        return os << "[]";
    }
    os << "vector" << '[';
    for (auto it = rhs.cbegin(); it != rhs.cend() - 1; ++it) {
        os << *it << ", ";
    }
    os << rhs.back() << ']';

    return os;
}

template<typename C, typename P, typename A>
std::ostream& operator<<(std::ostream& os, const std::set<C, P, A>& rhs) {
    if (rhs.empty()) {
        return os << "set[]";
    }
    os << "set" << '[';
    for (auto it = rhs.cbegin(); it != --rhs.cend(); ++it) {
        os << *it << ", ";
    }
    os << *(--rhs.end()) << ']';

    return os;
}

template<typename T, typename A>
std::ostream& operator<<(std::ostream& os, const custom_data_structures::circular_buffer<T, A>& rhs) {
    if (rhs.empty()) {
        return os << "circular[]";
    }
    os << "circular" << '[';
    for (auto it = rhs.cbegin(); it != rhs.cend() - 1; ++it) {
        os << *it << ", ";
    }
    os << rhs.back() << ']';

    return os;
}

template<typename T, typename D = std::less<T>, typename A = std::allocator<T>> 
static constexpr void(*ops[])(std::set<T, D, A>&, T) = { 
    [](std::set<T, D, A>& s, T v) { 
        s.insert(v); 
    },
    [](std::set<T, D, A>& s, T v) {
        auto it = s.find(v);
        if (it != s.end()) {
            s.erase(it);
        }
    }
}; 

int main() {
    timer global("Global timer");
    int times = 10000000;
    //std::vector<int, pool_allocator<int>> vector(pool_allocator<int>(16, 1024));
    
    //std::deque<char, pool_allocator<char>> deque(pool_allocator<char>(16, 1024));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 1000000);
    std::uniform_int_distribution<> choice(0, 1);
    std::vector<std::pair<int, int>> operations(times);

    for (auto& [op, value] : operations) {
        op = choice(gen);
        value = distrib(gen);
    }

    {
        std::set<int, std::less<int>, pool_allocator<int>> set_with_custom_alloc(pool_allocator<int>(40, 1000000));
        timer alloc_timer("Custom allocator");
        for (auto [op, v] : operations) {
            ops<int, std::less<int>, pool_allocator<int>>[op](set_with_custom_alloc, v);
        }
    }

    {
        std::set<int> set_with_default_alloc;
        timer alloc_timer("Default allocator");
        for (auto [op, v] : operations) {
            ops<int>[op](set_with_default_alloc, v);
        }
    }

}
