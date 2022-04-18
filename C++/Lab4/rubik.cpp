#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <string_view>
#include <span>
#include <array>
#include <map>
#include <utility>
#include "rubik.hpp"

namespace rubik {
    std::istream& operator>>(std::istream& is, colors& c) {
        static const std::map<char, colors> to_color{
            {'R', RED},
            {'O', ORANGE},
            {'W', WHITE},
            {'Y', YELLOW},
            {'B', BLUE},
            {'G', GREEN}
        };

        char sym = is.get();
        if (is.eof()) {
            return is;
        }

        if (to_color.contains(sym)) {
            c = to_color.at(sym);
        }
        else {
            is.setstate(std::ios_base::failbit);
        }

        return is;
    }


    sides opposing_side(sides side) {
        if (side < TOP) {
            return sides((side + 2) % 4);
        }
        return side == TOP ? BOTTOM : TOP;
    }

    const std::array<sides, 2> order_edge(sides first, sides second) {
        if (first > LEFT) {
            return {first, second};
        }
        if (second > LEFT) {
            return {second, first};
        }

        if (first == LEFT && second == FRONT) {
            return {LEFT, FRONT};
        }
        if (first < second) {
            return {first, second};
        }
        else {
            return {second, first};
        }
    }

    const std::array<sides, 3> order_corner(sides first, sides second, sides third) {
        if (first > LEFT) {
            auto remaining = order_edge(second, third);
            return {remaining[0], first, remaining[1]};
        }

        if (second > LEFT) {
            auto remaining = order_edge(first, third);
            return {remaining[0], second, remaining[1]};
        }

        if (third > LEFT) {
            auto remaining = order_edge(first, second);
            return {remaining[0], third, remaining[1]};
        }

        throw std::invalid_argument("One side must be either TOP or BOTTOM");
    }

    std::array<rubik_cube::edge, 12> rubik_cube::get_edges() const {
        return {
            edge({miniside{TOP, _state[TOP][7]}, miniside{FRONT, _state[FRONT][1]}}),
            edge({miniside{TOP, _state[TOP][5]}, miniside{RIGHT, _state[RIGHT][1]}}),
            edge({miniside{TOP, _state[TOP][1]}, miniside{BACK, _state[BACK][1]}}),
            edge({miniside{TOP, _state[TOP][3]}, miniside{LEFT, _state[LEFT][1]}}),

            edge({miniside{FRONT, _state[FRONT][5]}, miniside{RIGHT, _state[RIGHT][3]}}),
            edge({miniside{RIGHT, _state[RIGHT][5]}, miniside{BACK, _state[BACK][3]}}),
            edge({miniside{BACK, _state[BACK][5]}, miniside{LEFT, _state[LEFT][3]}}),
            edge({miniside{LEFT, _state[LEFT][5]}, miniside{FRONT, _state[FRONT][3]}}),

            edge({miniside{BOTTOM, _state[BOTTOM][1]}, miniside{FRONT, _state[FRONT][7]}}),
            edge({miniside{BOTTOM, _state[BOTTOM][5]}, miniside{RIGHT, _state[RIGHT][7]}}),
            edge({miniside{BOTTOM, _state[BOTTOM][7]}, miniside{BACK, _state[BACK][7]}}),
            edge({miniside{BOTTOM, _state[BOTTOM][3]}, miniside{LEFT, _state[LEFT][7]}})
        };
    }

    std::array<rubik_cube::corner, 8> rubik_cube::get_corners() const {
        return {
            corner({miniside{LEFT, _state[LEFT][2]}, miniside{TOP, _state[TOP][6]}, miniside{FRONT, _state[FRONT][0]}}),
            corner({miniside{FRONT, _state[FRONT][2]}, miniside{TOP, _state[TOP][8]}, miniside{RIGHT, _state[RIGHT][0]}}),
            corner({miniside{RIGHT, _state[RIGHT][2]}, miniside{TOP, _state[TOP][2]}, miniside{BACK, _state[BACK][0]}}),
            corner({miniside{BACK, _state[BACK][2]}, miniside{TOP, _state[TOP][0]}, miniside{LEFT, _state[LEFT][0]}}),

            corner({miniside{LEFT, _state[LEFT][8]}, miniside{BOTTOM, _state[TOP][0]}, miniside{FRONT, _state[FRONT][6]}}),
            corner({miniside{FRONT, _state[FRONT][8]}, miniside{BOTTOM, _state[TOP][2]}, miniside{RIGHT, _state[RIGHT][6]}}),
            corner({miniside{RIGHT, _state[RIGHT][8]}, miniside{BOTTOM, _state[TOP][8]}, miniside{BACK, _state[BACK][6]}}),
            corner({miniside{BACK, _state[BACK][8]}, miniside{BOTTOM, _state[TOP][6]}, miniside{LEFT, _state[LEFT][6]}}),
        };
    }
    
    rubik_cube::rubik_cube() {
        for (int side = 0; side < 6; side++) {
            _state[side].fill((colors)side);
        }
    }
    rubik_cube::rubik_cube(const std::string& scramble) {

        std::istringstream stream(scramble);

        if (scramble.length() != 9*6) {
            throw std::invalid_argument("Scramble string must be 54 characters long");
        }

        for (auto& side : _state) {
            for (auto& pos : side) {
                stream >> pos;
            }
        }

        auto check_count = [this](std::initializer_list<int> indexes, int desirable_count) {
            std::array<int, 6> colors_count{};
            
            for (auto& side : _state) {
                for (auto i : indexes) {
                    colors_count[side[i]]++;
                }
            }
    
            if (std::any_of(colors_count.begin(), colors_count.end(), 
                        [&](int c) { return c != desirable_count; })) {
                throw std::invalid_argument("Invalid scramble");
            }
        };

        // Centers
        check_count({4}, 1);
        // Corners
        check_count({0, 2, 6, 8}, 4);
        // Edges
        check_count({1, 3, 5, 7}, 4);
        
        static const std::map<colors, sides> color_side = {
            {_state[FRONT][4], FRONT},
            {_state[TOP][4], TOP},
            {_state[BOTTOM][4], BOTTOM},
            {_state[BACK][4], BACK},
            {_state[LEFT][4], LEFT},
            {_state[RIGHT][4], RIGHT},
        };

        std::map<std::array<colors, 2>, bool> edge_uniqueness;
        std::map<std::array<colors, 3>, bool> corner_uniqueness;

        int edge_parity = 0;
        int corner_parity = 0;
            
        /// Minicube's original permutation (in solved state)
        /// 
        /// Lower layer
        ///  7   6   5
        ///  8   -   4
        ///  1   2   3
        /// 
        /// Middle layer
        ///  12   -  11
        ///  -       -
        ///  9   -   10
        ///
        /// Higher layer
        ///  19  18  17
        ///  20  -   16
        ///  13  14  15
        std::array<int, 20> permutation{};

        static const std::map<std::array<sides, 2>, int> edge_permutation = {
            {{BOTTOM, FRONT}, 2},
            {{BOTTOM, RIGHT}, 4},
            {{BOTTOM, BACK}, 6},
            {{BOTTOM, LEFT}, 8},

            {{LEFT, FRONT}, 9},
            {{FRONT, RIGHT}, 10},
            {{RIGHT, BACK}, 11},
            {{BACK, LEFT}, 12},

            {{TOP, FRONT}, 14},
            {{TOP, RIGHT}, 16},
            {{TOP, BACK}, 18},
            {{TOP, LEFT}, 20},
        };

        static const std::map<std::array<sides, 3>, int> corner_permutation = {
            {{LEFT, BOTTOM, FRONT}, 1},
            {{FRONT, BOTTOM, RIGHT}, 3},
            {{RIGHT, BOTTOM, BACK}, 5},
            {{BACK, BOTTOM, LEFT}, 7},

            {{LEFT, TOP, FRONT}, 13},
            {{FRONT, TOP, RIGHT}, 15},
            {{RIGHT, TOP, BACK}, 17},
            {{BACK, TOP, LEFT}, 19},
        };

        static auto is_top_bottom_side = [](sides side) { return side > LEFT; };

        // Check edge existance
        
        // X - TOP and BOTTOM
        // Y - LEFT and RIGHT
        // Z - FRONT and BACK

        // NOTE: TOP and BOTTOM are always the first
        // Edges
        for (auto edge : get_edges()) {
            auto [first, second] = edge.sides;
            if (color_side.at(first.color) == opposing_side(color_side.at(second.color)) ||
                    first.color == second.color) {
                throw std::invalid_argument("Invalid scramble");
            }
            auto& is_present = edge_uniqueness[{
                std::min(first.color, second.color), 
                std::max(first.color, second.color)
            }];
            if (is_present) {
                throw std::invalid_argument("Invalid scramble");
            }
            is_present = true;

            // Parity
            
            // If edge with an X side
            if (is_top_bottom_side(color_side.at(first.color)) ||
                is_top_bottom_side(color_side.at(second.color))) {
                auto x_side = is_top_bottom_side(color_side.at(first.color)) ?
                    first : second;
                if (x_side.side == LEFT || x_side.side == RIGHT) {
                    edge_parity++;
                }
            }
            else {
                auto z_side = color_side.at(first.color) == FRONT || color_side.at(first.color) == BACK ?
                    first : second;
                if (!(color_side.at(first.color) == first.side && color_side.at(second.color) == second.side) &&
                        !is_top_bottom_side(z_side.side)) {
                    edge_parity++;
                }
            }

            // Permutation
            permutation[edge_permutation.at({first.side, second.side}) - 1] = 
                edge_permutation.at(order_edge(color_side.at(first.color), color_side.at(second.color)));
        }
        
        // Check corners

        for (auto corner : get_corners()) {
            // Existance (colors)
            auto x_side = std::find_if(corner.sides.begin(), corner.sides.end(), 
                        [](auto item) { return is_top_bottom_side(color_side.at(item.color)); });
            
            if (x_side == corner.sides.end()) {
                throw std::invalid_argument("Invalid scramble");
            }

            auto other = std::find_if(corner.sides.begin(), corner.sides.end(),
                    [](auto item) { return !is_top_bottom_side(color_side.at(item.color)); });

            auto and_another = std::find_if(corner.sides.rbegin(), corner.sides.rend(),
                    [](auto item) { return !is_top_bottom_side(color_side.at(item.color)); });

            if (opposing_side(color_side.at(other->color)) == color_side.at(and_another->color) ||
                    other->color == and_another->color) {
                throw std::invalid_argument("Invalid scramble");
            }

            auto& is_present = corner_uniqueness[{
                    std::min(other->color, and_another->color),
                    std::max(other->color, and_another->color),
                    x_side->color
                }];
            if (is_present) {
                throw std::invalid_argument("Invalid scramble");
            }
            is_present = true;

            // Corner parity
            if (!is_top_bottom_side(x_side->side)) {
                // First side is leftier, second is an X side and third side is rightier
                if (corner.sides[0].color == x_side->color) {
                    corner_parity++;
                }
                else {
                    corner_parity--;
                }
            }

            permutation[corner_permutation.at(order_corner(x_side->side, 
                                                           other->side, 
                                                           and_another->side)) - 1] =
                corner_permutation.at(order_corner(
                            color_side.at(x_side->color), 
                            color_side.at(other->color), 
                            color_side.at(and_another->color)
                        ));
        }

        // Count inversions within permutation array, if even count, then valid, otherwise no.
        
        int inversions = 0;

        for (auto it = permutation.cbegin(); it != permutation.cend() - 1; ++it) {
            if (*it > *(it + 1)) {
                inversions++;
            }
        }
        if (inversions % 2 != 0) {
            throw std::invalid_argument("Permutation parity test failed");
        }

        if (corner_parity % 3 != 0) {
            throw std::invalid_argument("Corner parity test failed");
        }

        if (edge_parity % 2 != 0) {
            throw std::invalid_argument("Edge parity test failed");
        }

        // It's done.
    }


    const state_array& rubik_cube::get_state() const {
        return _state;
    }

    cube_printer::cube_printer(const rubik_cube& cube, const std::string& scheme) : _cube(cube.get_state()), _scheme(scheme) {
        using namespace std::literals;

        static std::string_view valid_chars("FRULBD \n");

        for (auto c : "FRULBD"sv) {
            if (std::count(_scheme.cbegin(), _scheme.cend(), c) != 1) {
                throw std::invalid_argument("Scheme must contain only one side character");
            }
        }
        if (std::any_of(_scheme.cbegin(), _scheme.cend(), [&](char c) {
                    return valid_chars.find(c) == std::string_view::npos;
                    })) {
            throw std::invalid_argument("Scheme must contain only valid characters");
        }
    }

    std::ostream& operator<<(std::ostream& os, const cube_printer& p) {
        static const std::map<char, sides> sides{
            {'F', FRONT},
            {'R', RIGHT},
            {'U', TOP},
            {'L', LEFT},
            {'D', BOTTOM},
            {'B', BACK}
        };
        std::istringstream str(p._scheme);
        for (std::string line; std::getline(str, line);) {
            for (int part = 0; part < 3; part++) {
                for (auto c : line) {
                    if (c == ' ') {
                        p.print_blank_row(os);
                        continue;
                    }
                    p.print_row(os, std::span<const colors, 3>(
                                p._cube.at(sides.at(c)).data() + 3*part, 3));
                }
                os << '\n';
            }
        }
        return os;
    }

    color_printer::color_printer(const rubik_cube& cube, const std::string& scheme) 
        : cube_printer(cube, scheme), _symbols({
                    {RED, '1'},
                    {ORANGE, '5'},
                    {WHITE, '7'},
                    {YELLOW, '3'},
                    {BLUE, '4'},
                    {GREEN, '2'}
                }) {}

    color_printer::color_printer(const rubik_cube& cube, const std::string& scheme, const std::map<const colors, char>& symbols) 
        : cube_printer(cube, scheme), _symbols(symbols) {
        for (int color = 0; color < 6; color++) {
            if (_symbols.count((colors)color) != 1) {
                throw std::invalid_argument("Map must contain one character for each color");
            }
        }
    }

    void color_printer::print_row(std::ostream& os, std::span<const colors, 3> row) const {
        for (auto pos : row) {
            os << "\033[4" << _symbols.at(pos) << "m  ";
        }
        os << "\033[0m";
    }

    void color_printer::print_blank_row(std::ostream& os) const {
        os << "      ";
    }

    text_printer::text_printer(const rubik_cube& cube, const std::string& scheme) 
        : cube_printer(cube, scheme), _symbols({
                    {RED, 'R'},
                    {ORANGE, 'O'},
                    {WHITE, 'W'},
                    {YELLOW, 'Y'},
                    {BLUE, 'B'},
                    {GREEN, 'G'}
                }) {}

    text_printer::text_printer(const rubik_cube& cube, const std::string& scheme, const std::map<const colors, char>& symbols) 
        : cube_printer(cube, scheme), _symbols(symbols) {
        for (int color = 0; color < 6; color++) {
            if (_symbols.count((colors)color) != 1) {
                throw std::invalid_argument("Map must contain one character for each color");
            }
        }
    }

    void text_printer::print_row(std::ostream& os, std::span<const colors, 3> row) const {
        for (auto color : row) {
            os << _symbols.at(color);
        }
    }

    void text_printer::print_blank_row(std::ostream& os) const {
        os << "   ";
    }
}
