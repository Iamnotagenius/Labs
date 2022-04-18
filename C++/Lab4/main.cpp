#include <iostream>
#include <algorithm>
#include <random>

#include "rubik.hpp"

using std::cout;
using namespace rubik;
int main() {
    auto letters = "ROWYBG";
    std::mt19937 g;
    std::uniform_int_distribution<int> gen(0, 5);
    std::array<char, 55> scramble;
    std::generate(scramble.begin(), scramble.end() - 1, [&]() { return letters[gen(g)]; } );
    scramble[54] = '\0';

    rubik_cube cube(scramble.data());
    auto scheme = " U\nLFRB\n D";
    cout << "Text printer:\n"
         << text_printer(cube, scheme)
         << "Color printer:\n"
         << color_printer(cube, scheme);
    return 0;
}
