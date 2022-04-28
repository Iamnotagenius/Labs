#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <cli/cli.h>
#include <cli/loopscheduler.h>
#include <cli/clilocalsession.h>

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
    rubik_cube cube;
    auto printer = std::make_unique<color_printer>(cube);
    auto menu = std::make_unique<cli::Menu>("rubix");
    menu->Insert(
        "read",
        [&cube](std::ostream& os, const std::string& filename) {
            std::ifstream file(filename);
            try {
                rubik_cube tmp(file);
                cube = tmp;
            } catch (std::invalid_argument e) {
                cout << e.what() << '\n';
            }
        },
        "Read scramble from a file"
    );

    std::initializer_list<std::pair<std::string_view, std::function<void(rubik_cube&)>>> moves = {
        {"F", &rubik_cube::F},
        {"R", &rubik_cube::R},
        {"U", &rubik_cube::U},
        {"B", &rubik_cube::B},
        {"L", &rubik_cube::L},
        {"D", &rubik_cube::D}
    };
    for (auto pair : moves) {
        menu->Insert(
            std::string(pair.first),
            [&](std::ostream& os) {
                std::get<1>(pair)(cube);
                os << *printer;
            },
            std::string(pair.first) + " turn"
        );
    }

    cli::LoopScheduler loop;

    cli::Cli cli(std::move(menu));

    cli::CliLocalTerminalSession session(cli, loop, std::cout);
    loop.Run();
    return 0;
}
