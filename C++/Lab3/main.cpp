#include <ostream>
#include <iostream>
#include <iterator>
#include <functional>
#include "circular.hpp"
#include "algos.hpp"

#define PRINT_DEBUG(arg) std::cerr << "[" \
                                   << __FILE__ \
                                   << ":" \
                                   << __LINE__ \
                                   << "] " \
                                   << arg \
                                   << '\n';

using namespace custom_data_structures;
using namespace generic_algorithms;

template<typename T>
std::ostream& operator<<(std::ostream& lhs, const circular_buffer<T>& rhs) {
    if (rhs.size() == 0) {
        lhs << "[]";
        return lhs;
    }
    lhs << '[';
    for (auto it = rhs.cbegin(); it != rhs.cend() - 1; ++it) {
        lhs << *it << ", ";
    }
    lhs << rhs.back() << ']';

    return lhs;
}

class test {
    private:
        int x, y;
        char *data;
    public:
        test(int xa, int ya) : x(xa), y(ya) {
            std::cerr << "Constructed with x = " << x << " and y = " << y << '\n';
            data = new char;
        }
        test(const test& other) : x(other.x), y(other.y) {
            std::cerr << "Copied (" << x << ", " << y << ")\n";
            data = new char;
        }
        test(test&& other) : 
            x(std::move(other.x)), y(std::move(other.y)),
            data(std::move(other.data)) {
            other.data = nullptr;
            std::cerr << "Moved (" << x << ", " << y << ")\n";
        }
        ~test() {
            std::cerr << "Destructed (" << x << ", " << y << ")\n";
            delete data;
        }

        friend std::ostream& operator<<(std::ostream& lhs, const test& rhs) {
            lhs << "Test(" << rhs.x << ", " << rhs.y << ')';
            return lhs;
        }
};

void test1() {
    circular_buffer<test> first(2);
        first.push_back(test(5, 6));
        PRINT_DEBUG(first);
        first.emplace_back(3, 4);
        PRINT_DEBUG(first);
        test t(1, 2);
        first.push_back(t);
        PRINT_DEBUG(first);
        test t2(7, 8);
        first.push_back(std::move(t2));
        PRINT_DEBUG(first);
        first.emplace_front(10, 11);
        PRINT_DEBUG(first);
        first.pop_front();
        PRINT_DEBUG(first);
        
        circular_buffer<int> second(10);
        PRINT_DEBUG(second);
        
        second.push_back(5);
        PRINT_DEBUG(second);
        second.push_back(6);
        PRINT_DEBUG(second);
        second.push_back(7);
        PRINT_DEBUG(second);
        second.push_back(8);
        PRINT_DEBUG(second);
        second.push_front(1);
        PRINT_DEBUG(second);
        second.push_front(9);
        PRINT_DEBUG(second);
        second.back() = 2;
        PRINT_DEBUG(second);
        second.pop_back();
        PRINT_DEBUG(second);
        second.pop_front();
        PRINT_DEBUG(second);
        second.pop_front();
        PRINT_DEBUG(second);
        
        PRINT_DEBUG(second.size());
        for (size_t i = 0; i <= 6; ++i) {
            PRINT_DEBUG("first[" << i << "]: " << second[i]);
        }
    
        auto it = second.begin();
        it += 2;
        *it = 3;
        *(it - 1) = 2;
        *(1 + it) = 1;
        PRINT_DEBUG(second);
        std::copy(second.crbegin(), second.crend(), std::ostream_iterator<int>(std::cerr, ", "));
        std::cerr << '\n';
        PRINT_DEBUG(it - second.begin());
    
        for (auto& item : second) {
            item += 10;
        }
    
        PRINT_DEBUG(second);
        std::cerr << std::boolalpha;
        std::string s1("raar");
        std::string s2("radar");
        std::string s3("lol");
        std::string s4("ho");
        
        
        PRINT_DEBUG(is_palindrome(s1.begin(), s1.end(), std::equal_to<>()));
        PRINT_DEBUG(is_palindrome(s2.begin(), s2.end(), std::equal_to<>()));
        PRINT_DEBUG(is_palindrome(s3.begin(), s3.end(), std::equal_to<>()));
        PRINT_DEBUG(is_palindrome(s4.begin(), s4.end(), std::equal_to<>()));
        
}

int main() {
    circular_buffer<int> buf = {1, 2, 5, 3, 1};
    PRINT_DEBUG(buf);
    buf.push_front(7);
    buf.push_front(9);
    PRINT_DEBUG(buf);
    std::sort(buf.begin(), buf.end());
    PRINT_DEBUG(buf);

}
