#include <iostream>
#include <array>
#include "constexpr_polynom.hpp"

enum { compile_time = polynom<1, 1, 3>::eval<2>::value };


int main() {
    std::array<int, polynom<34, 54, -5, 62, 100, 43>::eval<3>::value> arr;
    std::cout << compile_time << std::endl
              << arr.max_size() << std::endl;
}
