#include <iostream>
#include <algorithm>
#include <random>

#include "rubik.hpp"

using std::cout;
using namespace rubik;
int main() {
    rubik_cube cube;

    cube.R().U().Ri().Ui().L().Ri().F().R().Fi().Li();
    //cube.L2().R2().D().L2().R2().U2().L2().R2().D().L2().R2();
    //cube.U2().B2().L2().Di().F2().L2().B2().Ui().R().Ui().F().U().F2().R().F().Ui().Fi();
    cout << color_printer(cube);
    old_pochmann(cube);

    return 0;
}
