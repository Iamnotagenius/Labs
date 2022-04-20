#include <iostream>
#include <algorithm>
#include <random>

#include "rubik.hpp"

using std::cout;
using namespace rubik;
int main() {
    state_array s = {{
        { // FRONT
            ORANGE, ORANGE, BLUE,
            GREEN, WHITE, WHITE,
            BLUE, BLUE, GREEN
        }, 
        { // RIGHT
            YELLOW, YELLOW, BLUE,
            GREEN, BLUE, BLUE,
            RED, GREEN, WHITE
        },
        { // BACK
            ORANGE, BLUE, BLUE,
            YELLOW, YELLOW, RED,
            GREEN, ORANGE, RED
        },
        { // LEFT
            WHITE, RED, GREEN,
            WHITE, GREEN, YELLOW,
            GREEN, WHITE, RED,
        },
        { // TOP
            RED, RED, WHITE,
            YELLOW, ORANGE, ORANGE,
            YELLOW, BLUE, ORANGE,
        },
        { // BOTTOM
            YELLOW, WHITE, YELLOW,
            ORANGE, RED, RED,
            WHITE, GREEN, ORANGE
        }
    }};
    rubik_cube cube;
    
    std::array<std::array<std::function<void(rubik_cube&)>, 3>, 6> rotations = {{
        {&rubik_cube::F, &rubik_cube::Fi, &rubik_cube::F2},
        {&rubik_cube::R, &rubik_cube::Ri, &rubik_cube::R2},
        {&rubik_cube::B, &rubik_cube::Bi, &rubik_cube::B2},
        {&rubik_cube::L, &rubik_cube::Li, &rubik_cube::L2},
        {&rubik_cube::U, &rubik_cube::Ui, &rubik_cube::U2},
        {&rubik_cube::D, &rubik_cube::Di, &rubik_cube::D2},
    }};

    auto scheme = " U\nLFRB\n D";

    cout << "Rotations tested as follows: X, Xi, X2, X2.\n";
    for (int side = 0; side < 6; side++) {
        cout << sides(side) << " side:\n";
        cout << color_printer(cube, scheme) << '\n';
        rotations[side][0](cube);
        cout << color_printer(cube, scheme) << '\n';
        rotations[side][1](cube);
        cout << color_printer(cube, scheme) << '\n';
        rotations[side][2](cube);
        cout << color_printer(cube, scheme) << '\n';
        rotations[side][2](cube);
        cout << color_printer(cube, scheme) << '\n';
    }
    return 0;
}
