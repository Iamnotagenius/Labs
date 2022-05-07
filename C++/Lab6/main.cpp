#include <iostream>
#include <type_traits>
#include <array>
#include "constexpr_polynom.hpp"

enum { compile_time = polynom<1, 1, 3>::eval<2>::value };

typedef polynom<34, -1, -5, 62, 0, 43> poly;

int main() {
    std::array<int, poly::eval<1>::value> arr;
    std::cout << poly::eval<5>::value << std::endl
              << arr.max_size() << std::endl
              << poly() << std::endl;

    // Static assert fails at compile time
    static_assert(poly::eval<5>::value == 142029, "Nope.");
}
