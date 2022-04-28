#include <iostream>
#include <algorithm>
#include <random>

#include "rubik.hpp"

using std::cout;
using namespace rubik;


std::ostream& operator<<(std::ostream& os, rubik::rubik_cube::miniside s) {
    os << s.color << " on " << s.side;
    return os;
}

template<typename T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& arr) {
    os << '[';
    for (auto it = arr.cbegin(); it != arr.cend() - 1; ++it)
        os << *it << ", ";
    os << arr.back() << ']';
    return os;
}

template<typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& rhs) {
    os << "{\n";
    for (auto [key, value] : rhs) {
        os << "    " << key << ": " << value << "\n";
    }
    os << "}\n";
    return os;
}

int main() {
    rubik_cube simple_swap;
    rubik_cube cycles;
    rubik_cube flipped;
    rubik_cube corner_swap;
    rubik_cube real_example;

    
    simple_swap.R().U().Ri().Ui().L().Ri().F().R().Fi().Li();
    cycles.L2().R2().D().L2().R2().U2().L2().R2().D().L2().R2();
    flipped.Mi().U().Mi().U().Mi().U2().M().U().M().U().M().U2();
    corner_swap.L2().F2().L().B().Li().F2().L().Bi().L();
    real_example.Ui().L2().Di().U().R2().Bi().Di().Ui().L2().B2().Ri().    
        Ui().Bi().Fi().L().U2().F().R2().Ui();
    //real_example.U().R().Ri().D().Ui();

    //cout << "Simple swap:\n" << color_printer(simple_swap);
    //old_pochmann(simple_swap);
    //cout << "Solved:\n" << color_printer(simple_swap) << "\n";
    //
    //cout << "New cycles:\n" << color_printer(cycles);
    //old_pochmann(cycles);
    //cout << "Solved:\n" << color_printer(cycles);
    //
    //cout << "Flipped:\n" << color_printer(flipped);
    //old_pochmann(flipped);
    //cout << "Solved:\n" << color_printer(flipped) << "\n";
    //
    //cout << "Corner swap:\n" << color_printer(corner_swap);
    //old_pochmann(corner_swap);
    //cout << "Solved:\n" << color_printer(corner_swap) << "\n";
    //
    cout << "Real Example:\n" << color_printer(real_example);
    auto solution = old_pochmann(real_example);
    cout << "Solved:\n" << color_printer(real_example) << '\n'
         << "Solution: " << solution << '\n';
    
    return 0;
}
