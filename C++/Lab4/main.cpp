#include <iostream>
#include <algorithm>
#include <random>

#include "rubik.hpp"

using std::cout;
using namespace rubik;
int main() {
    rubik_cube cube(33);

    std::array<std::array<std::function<void(rubik_cube&)>, 3>, 6> rotations = {{
        {&rubik_cube::F, &rubik_cube::Fi, &rubik_cube::F2},
        {&rubik_cube::R, &rubik_cube::Ri, &rubik_cube::R2},
        {&rubik_cube::B, &rubik_cube::Bi, &rubik_cube::B2},
        {&rubik_cube::L, &rubik_cube::Li, &rubik_cube::L2},
        {&rubik_cube::U, &rubik_cube::Ui, &rubik_cube::U2},
        {&rubik_cube::D, &rubik_cube::Di, &rubik_cube::D2},
    }};
    
    cout << color_printer(cube);

    return 0;
}
