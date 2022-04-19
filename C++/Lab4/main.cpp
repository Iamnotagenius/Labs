#include <iostream>
#include <algorithm>
#include <random>

#include "rubik.hpp"

using std::cout;
using namespace rubik;
int main() {
    rubik_cube cube({{
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
    }});

    auto scheme = " U\nLFRB\n D";
    cout << "Text printer:\n"
         << text_printer(cube, scheme)
         << "Color printer:\n"
         << color_printer(cube, scheme);
    return 0;
}
