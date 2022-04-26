#include <iostream>
#include <algorithm>
#include <random>

#include "rubik.hpp"

using std::cout;
using namespace rubik;
int main() {
    rubik_cube simple_swap;
    rubik_cube cycles;
    rubik_cube flipped;

    
    simple_swap.R().U().Ri().Ui().L().Ri().F().R().Fi().Li();
    cycles.L2().R2().D().L2().R2().U2().L2().R2().D().L2().R2();
    flipped.M().E().S().S().E();
    cout << color_printer(flipped);

    rubik_cube real_example;

    return 0;
}
