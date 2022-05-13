#include <iostream>
#include <type_traits>
#include <array>
#include "constexpr_polynom.hpp"

enum { compile_time = polynom<0, 1, 1, 1, 2, 3>::eval<2>::value };

typedef polynom<0, 34, 
                1, -1, 
                2, -5, 
                3, 62, 
                5, 43> poly;

typedef polynom<2, 1, 
                1, -4, 
                0, 3> quadratic;

int main() {
    std::array<int, poly::eval<1>::value> arr;
    std::cout << poly::eval<5>::value << std::endl
              << arr.max_size() << std::endl
              << polynom<10, 1, 0, 5>::eval<2>::value << std::endl
              << quadratic() << std::endl
              << poly() << std::endl;

     

    // Static assert fails at compile time
    static_assert(poly::eval<5>::value == 142029, "Nope.");
    static_assert(quadratic::eval<3>::value == 0, "Not root");

}
