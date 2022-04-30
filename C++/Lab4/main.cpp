#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <cli/cli.h>
#include <cli/loopscheduler.h>
#include <cli/clilocalsession.h>
#include <cli/colorprofile.h>

#include "rubik.hpp"
#include "../Lab3/circular.hpp"

using std::cout;
using namespace rubik;
using namespace custom_data_structures;


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

template<typename T>
std::ostream& operator<<(std::ostream& os, const circular_buffer<T>& arr) {
    for (auto it = arr.cbegin(); it != arr.cend() - 1; ++it)
        os << *it << " ";
    os << arr.back();
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
    rubik_cube cube;
    auto printer = std::make_unique<color_printer>(cube);
    auto menu = std::make_unique<cli::Menu>();
    circular_buffer<std::string_view> last_moves(5);
    
    std::map<colors, char> to_char = {
        {GREEN, 'G'},
        {RED, 'R'},
        {BLUE, 'B'},
        {ORANGE, 'O'},
        {WHITE, 'W'},
        {YELLOW, 'Y'}
    };

    std::map<char, colors> to_colors = {
        {'G', GREEN},
        {'R', RED},
        {'B', BLUE},
        {'O', ORANGE},
        {'W', WHITE},
        {'Y', YELLOW}
    };

    menu->Insert(
        "read",
        [&](std::ostream& os, const std::string& filename) {
            std::ifstream file(filename);
            try {
                rubik_cube tmp(file, to_colors);
                cube = tmp;
            } catch (std::exception& e) {
                cout << e.what() << '\n';
            }
        },
        "Read scramble from a file"
    );

    menu->Insert(
        "save",
        [&](std::ostream& os, const std::string& filename) {
            std::ofstream file(filename, std::ios_base::trunc);
            write_cube(file, cube, to_char);
        },
        "Save scramble to a file"
    );
    
    menu->Insert(
        "print",
        [&printer](std::ostream& os) {
            os << *printer;
        },
        "Print out the cube"
    );

    auto g = std::bind(&rubik_cube::F, cube);

    std::initializer_list<std::pair<std::string_view, std::function<rubik_cube&(rubik_cube&)>>> moves = {
        {"F", &rubik_cube::F },
        {"R", &rubik_cube::R },
        {"U", &rubik_cube::U },
        {"B", &rubik_cube::B },
        {"L", &rubik_cube::L },
        {"D", &rubik_cube::D },
        { "F2", &rubik_cube::F2 },
        { "R2", &rubik_cube::R2 },
        { "U2", &rubik_cube::U2 },
        { "L2", &rubik_cube::L2 },
        { "B2", &rubik_cube::B2 },
        { "D2", &rubik_cube::D2 },
    
        { "Fi", &rubik_cube::Fi },
        { "Ri", &rubik_cube::Ri },
        { "Ui", &rubik_cube::Ui },
        { "Li", &rubik_cube::Li },
        { "Bi", &rubik_cube::Bi },
        { "Di", &rubik_cube::Di },

        { "X", &rubik_cube::X },
        { "Y", &rubik_cube::Y },
        { "Z", &rubik_cube::Z },

        { "Xi", &rubik_cube::Xi },
        { "Yi", &rubik_cube::Yi },
        { "Zi", &rubik_cube::Zi },


        { "X2", &rubik_cube::X2 },
        { "Y2", &rubik_cube::Y2 },
        { "Z2", &rubik_cube::Z2 },

        { "M", &rubik_cube::M },
        { "E", &rubik_cube::E },
        { "S", &rubik_cube::S },

        { "Mi", &rubik_cube::Mi },
        { "Ei", &rubik_cube::Ei },
        { "Si", &rubik_cube::Si },

        { "M2", &rubik_cube::M2 },
        { "E2", &rubik_cube::E2 },
        { "S2", &rubik_cube::S2 }
    };
    
    for (auto pair : moves) {
        menu->Insert(
            std::string(pair.first),
            [&, pair](std::ostream& os) {
                pair.second(cube);
                last_moves.push_back(pair.first);
                os << *printer << "\033[1;32m" << last_moves;

            },
            std::string(pair.first) + " turn"
        );
    }

    menu->Insert(
        "solve",
        [&cube](std::ostream& os) {
            os << old_pochmann(cube) << '\n';
        },
        "Solve the cube with Old Pochmann's Algorithm and print out a solution"
    );

    cli::LoopScheduler loop;

    cli::SetColor();

    cli::Cli cli(std::move(menu));

    cli::CliLocalTerminalSession session(cli, loop, std::cout);

    session.ExitAction(
        [&loop] (std::ostream& os) {
           loop.Stop();
        }
    );
    

    loop.Run();
    loop.Stop();
    return 0;
}
