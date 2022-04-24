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
    flipped.U2().B2().L2().Di().F2().L2().B2().Ui().R().Ui().F().U().F2().R().F().Ui().Fi();

    cout << "Simple swap:\n" << color_printer(simple_swap);
    old_pochmann(simple_swap);

    cout << "Two cycles:\n" << color_printer(cycles);
    old_pochmann(cycles);

    cout << "Flipped edges:\n" << color_printer(flipped);
    old_pochmann(flipped);

    rubik_cube corner_swap;
    corner_swap.L2().F2().L().B().Li().F2().L().Bi().L();
    cout << "Corner swap:\n" << color_printer(corner_swap);
    old_pochmann(corner_swap);

    rubik_cube real_example;

    return 0;
}
