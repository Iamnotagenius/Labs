#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <random>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include "rubik.hpp"


std::ostream& operator<<(std::ostream& os, rubik::colors color) {
    using namespace rubik;
    static const std::map<rubik::colors, const char *> string_map = {
        {WHITE, "WHITE"},
        {BLUE, "BLUE"},
        {YELLOW, "YELLOW"},
        {ORANGE, "ORANGE"},
        {RED, "RED"},
        {GREEN, "GREEN"}
    };
    os << string_map.at(color);
    return os;
}

std::ostream& operator<<(std::ostream& os, rubik::sides side) {
    using namespace rubik;
    static const std::map<rubik::sides, const char *> string_map = {
        {FRONT, "FRONT"},
        {RIGHT, "RIGHT"},
        {BACK, "BACK"},
        {LEFT, "LEFT"},
        {TOP, "TOP"},
        {BOTTOM, "BOTTOM"}
    };
    os << string_map.at(side);
    return os;
}

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

        if ((first == LEFT && second == FRONT) ||
                (first == FRONT && second == LEFT)) {
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

            corner({miniside{LEFT, _state[LEFT][8]}, miniside{BOTTOM, _state[BOTTOM][0]}, miniside{FRONT, _state[FRONT][6]}}),
            corner({miniside{FRONT, _state[FRONT][8]}, miniside{BOTTOM, _state[BOTTOM][2]}, miniside{RIGHT, _state[RIGHT][6]}}),
            corner({miniside{RIGHT, _state[RIGHT][8]}, miniside{BOTTOM, _state[BOTTOM][8]}, miniside{BACK, _state[BACK][6]}}),
            corner({miniside{BACK, _state[BACK][8]}, miniside{BOTTOM, _state[BOTTOM][6]}, miniside{LEFT, _state[LEFT][6]}}),
        };
    }

    void rubik_cube::validate() {

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
        for (auto [first, second] : get_edges()) {
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
                auto other_side = x_side.side == first.side ? second : first;
                if ((x_side.side == LEFT || x_side.side == RIGHT || is_top_bottom_side(other_side.side)) &&
                                                                     !is_top_bottom_side(x_side.side)) {
                    edge_parity++;
                }
            }
            else {
                auto z_side = color_side.at(first.color) == FRONT || color_side.at(first.color) == BACK ?
                    first : second;
                if (!((color_side.at(first.color) == first.side || color_side.at(first.color) == opposing_side(first.side)) &&
                      (color_side.at(second.color) == second.side || color_side.at(second.color) == opposing_side(second.side))) &&
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
            auto x_side = std::find_if(corner.begin(), corner.end(),
                        [](auto item) { return is_top_bottom_side(color_side.at(item.color)); });

            if (x_side == corner.end()) {
                throw std::invalid_argument("Invalid scramble (X side is not present)");
            }

            auto other = std::find_if(corner.begin(), corner.end(),
                    [](auto item) { return !is_top_bottom_side(color_side.at(item.color)); });

            auto and_another = std::find_if(corner.rbegin(), corner.rend(),
                    [](auto item) { return !is_top_bottom_side(color_side.at(item.color)); });


            if (opposing_side(color_side.at(other->color)) == color_side.at(and_another->color) ||
                    other->color == and_another->color) {
                throw std::invalid_argument("Invalid scramble (Invalid color positions)");
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
                if (corner[1].side == TOP) {
                    if (corner[0].color == x_side->color) {
                        corner_parity++;
                    }
                    else {
                        corner_parity--;
                    }
                }
                else {
                    if (corner[0].color == x_side->color) {
                        corner_parity--;
                    }
                    else {
                        corner_parity++;
                    }
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
            for (auto jt = it + 1; jt != permutation.cend(); ++jt) {
                if (*it > *jt) {
                    inversions++;
                }
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
    rubik_cube::rubik_cube() {
        for (int side = 0; side < 6; side++) {
            _state[side].fill((colors)side);
        }
    }

    rubik_cube::rubik_cube(std::istream& stream, const std::map<char, colors>& color_map) {

        for (auto& side : _state) {
            for (auto& pos : side) {
                char sym;
                stream >> sym;
                pos = color_map.at(sym);
            }
        }
        validate();
    }

    rubik_cube::rubik_cube(std::istream& stream) :
        rubik_cube::rubik_cube(stream, {{
                    {'R', RED},
                    {'O', ORANGE},
                    {'W', WHITE},
                    {'Y', YELLOW},
                    {'B', BLUE},
                    {'G', GREEN}
                    }}) {}

    rubik_cube::rubik_cube(const state_array& scramble) : _state(scramble) {
        validate();
    }

    rubik_cube::rubik_cube(int times) : rubik_cube::rubik_cube() {
        static const std::array<std::function<void(rubik_cube&)>, 12> rotations = {
            &rubik_cube::F, &rubik_cube::R, &rubik_cube::U, &rubik_cube::L, &rubik_cube::B, &rubik_cube::D,
            &rubik_cube::Fi, &rubik_cube::Ri, &rubik_cube::Ui, &rubik_cube::Li, &rubik_cube::Bi, &rubik_cube::Di
        };
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<std::size_t> distrib(0, 11);

        for (int i = 0; i < times; i++) {
            auto rot = distrib(gen);
            rotations[rot](*this);
        }
    }

    const state_array& rubik_cube::get_state() const {
        return _state;
    }

    std::map<colors, sides> rubik_cube::get_centers() const {
        return {
            {_state[FRONT][4], FRONT},
            {_state[TOP][4], TOP},
            {_state[BOTTOM][4], BOTTOM},
            {_state[BACK][4], BACK},
            {_state[LEFT][4], LEFT},
            {_state[RIGHT][4], RIGHT},
        };
    }

    colors rubik_cube::get_center(sides side) const {
        return _state[side][4];
    }

    rubik_cube::edge rubik_cube::get_edge(sides first, sides second) const {
        if (first == opposing_side(second)) {
            throw std::invalid_argument("Sides must be adjacent");
        }
        static constexpr std::array<int, 4> bottom_indexes = {1, 5, 7, 3};
        static constexpr std::array<int, 4> top_indexes = {7, 5, 1, 3};
        
        if (first == BOTTOM) {
            return {rubik_cube::miniside{BOTTOM, _state[BOTTOM][bottom_indexes[second]]}, 
                rubik_cube::miniside{second, _state[second][7]}};
        }
        else if (second == BOTTOM) {
            return {rubik_cube::miniside{first, _state[first][7]},
                rubik_cube::miniside{BOTTOM, _state[BOTTOM][bottom_indexes[first]]}};
        }
        else if (first == TOP) {
            return {rubik_cube::miniside{TOP, _state[TOP][top_indexes[second]]}, 
                rubik_cube::miniside{second, _state[second][1]}};
        }
        else if (second == TOP) {
            return {rubik_cube::miniside{first, _state[first][1]},
                rubik_cube::miniside{TOP, _state[TOP][top_indexes[first]]}};
        }
        else if ((first + 1) % 4 == second) {
            return {miniside{first, _state[first][5]}, miniside{second, _state[second][3]}};
        }

        return {miniside{first, _state[first][3]}, miniside{second, _state[second][5]}};

    }

    rubik_cube::corner rubik_cube::get_corner(sides first, sides second, sides third) const {
        if (first == opposing_side(second) || first == opposing_side(third) || second == opposing_side(third)) {
            throw std::invalid_argument("Sides must be adjacent");
        }

        static const std::map<std::array<sides, 2>, int> top_indexes = {
            {{FRONT, RIGHT}, 8},
            {{RIGHT, BACK}, 2},
            {{BACK, LEFT}, 0},
            {{FRONT, LEFT}, 6}
        };
        static const std::map<std::array<sides, 2>, int> bottom_indexes = {
            {{FRONT, RIGHT}, 2},
            {{RIGHT, BACK}, 8},
            {{BACK, LEFT}, 6},
            {{FRONT, LEFT}, 0}
        };

        int x_side, left_side = -1, right_side = -1;
        std::array<sides, 3> in_order = {first, second, third};
        for (int i = 0; i < 3; i++) {
            if (in_order[i] == TOP || in_order[i] == BOTTOM) {
                x_side = i;
            }
            else {
                if (left_side != -1) {
                    right_side = i;
                }
                else {
                    left_side = i;
                }
            }
        }
        if (!(in_order[left_side] < in_order[right_side] || (in_order[left_side] == LEFT && in_order[right_side] == FRONT))) {
            std::swap(left_side, right_side);
        }

        corner res;
        
        for (int i = 0; i < 3; i++) {
            if (i == x_side) {
                auto& idx_map = x_side == TOP ? top_indexes : bottom_indexes;
                res[i] = miniside{in_order[i], 
                    _state[x_side][idx_map.at({std::min(in_order[(i + 1) % 3], in_order[(i + 2) % 3]), 
                                               std::max(in_order[(i + 1) % 3], in_order[(i + 2) % 3])})]};
            }
            else if (i == left_side) {
                res[i] = miniside{in_order[i], _state[in_order[i]][in_order[x_side] == TOP ? 2 : 8]};
            }
            else if (i == right_side) {
                res[i] = miniside{in_order[i], _state[in_order[i]][in_order[x_side] == TOP ? 0 : 6]};
            }
            else {
                throw std::logic_error("Some of the indexes were not initialized");
            }
        }

        return res;
    }

    // Rotations

    void rubik_cube::rotate_side(sides side, const std::array<int, 9>& rotate_map) {
        std::array<colors, 9> copy(_state[side]);

        for (int i = 0; i < 9; i++) {
            _state[side][i] = copy[rotate_map[i]];
        }

        static const std::array<std::array<int, 3>, 4> edges = {{
            {0, 1, 2},
            {2, 5, 8},
            {8, 7, 6},
            {6, 3, 0}
        }};

        using row = std::array<colors, 3>;
        using prow = std::array<colors*, 3>;

        if (side != TOP && side != BOTTOM) {
            auto& left_side = _state[((side - 1) % 4 + 4) % 4];
            prow to_left = {&left_side[2], &left_side[5], &left_side[8]};
            row from_left = {left_side[2], left_side[5], left_side[8]};

            auto& right_side = _state[(side + 1) % 4];
            prow to_right = {&right_side[0], &right_side[3], &right_side[6]};
            row from_right = {right_side[0], right_side[3], right_side[6]};

            prow to_top;
            if (side == RIGHT || side == LEFT) {
                to_top = {
                    &_state[TOP][edges.at(side)[2]],
                    &_state[TOP][edges.at(side)[1]],
                    &_state[TOP][edges.at(side)[0]]
                };
            }
            else {
                to_top = {
                    &_state[TOP][edges.at((side + 2) % 4)[2]],
                    &_state[TOP][edges.at((side + 2) % 4)[1]],
                    &_state[TOP][edges.at((side + 2) % 4)[0]]
                };
            }
            row from_top = {*to_top[0], *to_top[1], *to_top[2]};

            prow to_bottom = {
                &_state[BOTTOM][edges.at(side)[0]],
                &_state[BOTTOM][edges.at(side)[1]],
                &_state[BOTTOM][edges.at(side)[2]]
            };
            row from_bottom = {*to_bottom[0], *to_bottom[1], *to_bottom[2]};

            // clockwise
            if (rotate_map[0] == 6) {
                for (int i = 0; i < 3; i++) {
                    *to_right[i] = from_top[i];
                    *to_bottom[i] = from_right[2 - i];
                    *to_left[i] = from_bottom[i];
                    *to_top[i] = from_left[2 - i];
                }
            }
            else if (rotate_map[0] == 2) {
                for (int i = 0; i < 3; i++) {
                    *to_left[i] = from_top[2 - i];
                    *to_bottom[i] = from_left[i];
                    *to_right[i] = from_bottom[2 - i];
                    *to_top[i] = from_right[i];
                }
            }
            else if (rotate_map[0] == 8) {
                for (int i = 0; i < 3; i++) {
                    *to_left[i] = from_right[2 - i];
                    *to_bottom[i] = from_top[2 - i];
                    *to_right[i] = from_left[2 - i];
                    *to_top[i] = from_bottom[2 - i];
                }
            }
            else {
                throw std::logic_error("Wrong rotate map");
            }
        }
        else {
            std::array<int, 3> indexes;
            if (side == TOP)
                indexes = {0, 1, 2};
            else
                indexes = {6, 7, 8};

            std::array<row, 4> copy = {{
                {_state[FRONT][indexes[0]], _state[FRONT][indexes[1]], _state[FRONT][indexes[2]]},
                {_state[RIGHT][indexes[0]], _state[RIGHT][indexes[1]], _state[RIGHT][indexes[2]]},
                {_state[BACK][indexes[0]], _state[BACK][indexes[1]], _state[BACK][indexes[2]]},
                {_state[LEFT][indexes[0]], _state[LEFT][indexes[1]], _state[LEFT][indexes[2]]}
            }};

            for (int outer_side = 0; outer_side < 4; outer_side++) {
                int corresponding_side;

                // Rotation depends on side
                int direction = side == BOTTOM ? 1 : -1;

                // clockwise
                if (rotate_map[0] == 6 ) {
                    corresponding_side = ((outer_side + direction) % 4 + 4) % 4;
                }
                // counterclockwise
                else if (rotate_map[0] == 2) {
                    corresponding_side = ((outer_side - direction) % 4 + 4) % 4;
                }
                // twice
                else {
                    corresponding_side = (outer_side + 2) % 4;
                }

                for (int i = 0; i < 3; i++) {
                    _state[corresponding_side][indexes[i]] = copy[outer_side][i];
                }
            }
        }
    }

    rubik_cube& rubik_cube::F() {
        rotate_side(FRONT, rotate_clockwise_map);
        return *this;
    }

    rubik_cube& rubik_cube::R() {
        rotate_side(RIGHT, rotate_clockwise_map);
        return *this;
    }

    rubik_cube& rubik_cube::U() {
        rotate_side(TOP, rotate_clockwise_map);
        return *this;
    }

    rubik_cube& rubik_cube::L() {
        rotate_side(LEFT, rotate_clockwise_map);
        return *this;
    }

    rubik_cube& rubik_cube::B() {
        rotate_side(BACK, rotate_clockwise_map);
        return *this;
    }

    rubik_cube& rubik_cube::D() {
        rotate_side(BOTTOM, rotate_clockwise_map);
        return *this;
    }

    rubik_cube& rubik_cube::F2() {
        rotate_side(FRONT, rotate_twice_map);
        return *this;
    }

    rubik_cube& rubik_cube::R2() {
        rotate_side(RIGHT, rotate_twice_map);
        return *this;
    }

    rubik_cube& rubik_cube::U2() {
        rotate_side(TOP, rotate_twice_map);
        return *this;
    }

    rubik_cube& rubik_cube::L2() {
        rotate_side(LEFT, rotate_twice_map);
        return *this;
    }

    rubik_cube& rubik_cube::B2() {
        rotate_side(BACK, rotate_twice_map);
        return *this;
    }

    rubik_cube& rubik_cube::D2() {
        rotate_side(BOTTOM, rotate_twice_map);
        return *this;
    }

    rubik_cube& rubik_cube::Fi() {
        rotate_side(FRONT, rotate_counterclockwise_map);
        return *this;
    }

    rubik_cube& rubik_cube::Ri() {
        rotate_side(RIGHT, rotate_counterclockwise_map);
        return *this;
    }

    rubik_cube& rubik_cube::Ui() {
        rotate_side(TOP, rotate_counterclockwise_map);
        return *this;
    }

    rubik_cube& rubik_cube::Li() {
        rotate_side(LEFT, rotate_counterclockwise_map);
        return *this;
    }

    rubik_cube& rubik_cube::Bi() {
        rotate_side(BACK, rotate_counterclockwise_map);
        return *this;
    }

    rubik_cube& rubik_cube::Di() {
        rotate_side(BOTTOM, rotate_counterclockwise_map);
        return *this;
    }

    bool is_same_edges(rubik_cube::edge lhs, rubik_cube::edge rhs) {
        return (lhs[0].color == rhs[0].color && lhs[1].color == rhs[1].color) ||
               (lhs[0].color == rhs[1].color && lhs[1].color == rhs[0].color);
    }


    std::string old_pochmann(rubik_cube& cube) {
        
        auto color_sides = cube.get_centers();

        /*
         * Memorization
         *
         *
         *       | A A B |
         *       | D   B |
         *       | D C C |
         *
         * E E F | I I J | M M N | Q Q R
         * H   F | L   J | P   N | T   R
         * H G G | L K K | P O O | T S S
         *
         *       | U U V |
         *       | X   V |
         *       | X W W |
        */

        const std::map<std::array<colors, 2>, char> edge_letters = {
            {{cube.get_center(TOP),     cube.get_center(BACK)},     'A'},
            {{cube.get_center(TOP),     cube.get_center(RIGHT)},    'B'},
            {{cube.get_center(TOP),     cube.get_center(FRONT)},    'C'},
            {{cube.get_center(TOP),     cube.get_center(LEFT)},     'D'},
            {{cube.get_center(LEFT),    cube.get_center(TOP)},      'E'},
            {{cube.get_center(LEFT),    cube.get_center(FRONT)},    'F'},
            {{cube.get_center(LEFT),    cube.get_center(BOTTOM)},   'G'},
            {{cube.get_center(LEFT),    cube.get_center(BACK)},     'H'},
            {{cube.get_center(FRONT),   cube.get_center(TOP)},      'I'},
            {{cube.get_center(FRONT),   cube.get_center(RIGHT)},    'J'},
            {{cube.get_center(FRONT),   cube.get_center(BOTTOM)},   'K'},
            {{cube.get_center(FRONT),   cube.get_center(LEFT)},     'L'},
            {{cube.get_center(RIGHT),   cube.get_center(TOP)},      'M'},
            {{cube.get_center(RIGHT),   cube.get_center(BACK)},     'N'},
            {{cube.get_center(RIGHT),   cube.get_center(BOTTOM)},   'O'},
            {{cube.get_center(RIGHT),   cube.get_center(FRONT)},    'P'},
            {{cube.get_center(BACK),    cube.get_center(TOP)},      'Q'},
            {{cube.get_center(BACK),    cube.get_center(LEFT)},     'R'},
            {{cube.get_center(BACK),    cube.get_center(BOTTOM)},   'S'},
            {{cube.get_center(BACK),    cube.get_center(RIGHT)},    'T'},
            {{cube.get_center(BOTTOM),  cube.get_center(FRONT)},    'U'},
            {{cube.get_center(BOTTOM),  cube.get_center(RIGHT)},    'V'},
            {{cube.get_center(BOTTOM),  cube.get_center(BACK)},     'W'},
            {{cube.get_center(BOTTOM),  cube.get_center(LEFT)},     'X'},
        };

        const std::map<char, rubik_cube::edge> letters_to_edge = {
            {'A', cube.get_edge(TOP, BACK)},
            {'B', cube.get_edge(TOP, RIGHT)},
            {'C', cube.get_edge(TOP, FRONT)},
            {'D', cube.get_edge(TOP, LEFT)},
            {'E', cube.get_edge(LEFT, TOP)},
            {'F', cube.get_edge(LEFT, FRONT)},
            {'G', cube.get_edge(LEFT, BOTTOM)},
            {'H', cube.get_edge(LEFT, BACK)},
            {'I', cube.get_edge(FRONT, TOP)},
            {'J', cube.get_edge(FRONT, RIGHT)},
            {'K', cube.get_edge(FRONT, BOTTOM)},
            {'L', cube.get_edge(FRONT, LEFT)},
            {'M', cube.get_edge(RIGHT, TOP)},
            {'N', cube.get_edge(RIGHT, BACK)},
            {'O', cube.get_edge(RIGHT, BOTTOM)},
            {'P', cube.get_edge(RIGHT, FRONT)},
            {'Q', cube.get_edge(BACK, TOP)},
            {'R', cube.get_edge(BACK, LEFT)},
            {'S', cube.get_edge(BACK, BOTTOM)},
            {'T', cube.get_edge(BACK, RIGHT)},
            {'U', cube.get_edge(BOTTOM, FRONT)},
            {'V', cube.get_edge(BOTTOM, RIGHT)},
            {'W', cube.get_edge(BOTTOM, BACK)},
            {'X', cube.get_edge(BOTTOM, LEFT)},
        };
        std::string edges_mem;

        // When order does not matter
        auto sorted_edge_colors = [](rubik_cube::edge e) -> std::array<colors, 2> {
            return {std::min(e[0].color, e[1].color),
                    std::max(e[0].color, e[1].color)};
        };

        // When order does matter
        auto ordered_edge_colors = [](rubik_cube::edge e) -> std::array<colors, 2> {
            return {e[0].color, e[1].color};
        };
        
        // Buffer is UR edge
        auto is_buffer = [&cube](std::array<colors, 2> c) {
            return std::find(c.begin(),
                             c.end(), 
                             cube.get_center(TOP)) != c.end() && 
                   std::find(c.begin(),
                             c.end(), 
                             cube.get_center(RIGHT)) != c.end();
        };
        rubik_cube::edge current = cube.get_edge(TOP, RIGHT);
        rubik_cube::edge first(current);
        std::map<std::array<colors, 2>, bool> solved_edges;
        for (auto edge : cube.get_edges()) {
            solved_edges.insert({sorted_edge_colors(edge), 
                    edge[0].color == cube.get_center(edge[0].side) &&
                    edge[1].color == cube.get_center(edge[1].side)
                });
        }

        do {
            // Using maps, locate new edges and memorize desired locations
            do {
                auto letter = edge_letters.at(ordered_edge_colors(current));
                solved_edges[sorted_edge_colors(current)] = true;
                // No memorization for buffer edge
                if (is_buffer(sorted_edge_colors(current))) {
                        break;
                }
                edges_mem.push_back(letter);
                current = letters_to_edge.at(letter);
    
            } while (!is_same_edges(current, first));

            if (!is_buffer(sorted_edge_colors(current))) {
                auto letter = edge_letters.at(ordered_edge_colors(current));
                edges_mem.push_back(letter);
            }

            // Find next cycle, if exists
            for (auto edge : cube.get_edges()) {
                if (!solved_edges[sorted_edge_colors(edge)]) {
                    current = edge;
                    first = edge;
                    break;
                }
            }

        } while (std::any_of(solved_edges.begin(), 
                             solved_edges.end(), 
                             [](auto kv) { return !std::get<1>(kv); }));
    

        std::cout << edges_mem << '\n';



        std::string moves;

        return moves;
    }



    cube_printer::cube_printer(const rubik_cube& cube) :
        _cube(cube.get_state()), _scheme(" U\nLFRB\n D") {}


    cube_printer::cube_printer(const rubik_cube& cube, std::string_view scheme) :
        _cube(cube.get_state()), _scheme(scheme) {
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
        std::string_view lines(p._scheme);

        while (!lines.empty()) {
            auto line = lines.substr(0, lines.find('\n'));
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
            lines.remove_prefix(lines.find('\n') + 1);
        }
        return os;
    }

    color_printer::color_printer(const rubik_cube& cube) : color_printer(cube, " U\nLFRB\n D\n") {}

    color_printer::color_printer(const rubik_cube& cube, std::string_view scheme)
        : cube_printer(cube, scheme), _colors({
                    {RED, {220, 20, 60}},
                    {ORANGE, {255, 140, 0}},
                    {WHITE, {255, 255, 255}},
                    {YELLOW, {251, 236, 93}},
                    {BLUE, {119, 181, 254}},
                    {GREEN, {119, 221, 119}}
                }) {}

    color_printer::color_printer(const rubik_cube& cube, std::string_view scheme, const std::map<colors, std::array<int, 3>>& colors)
        : cube_printer(cube, scheme), _colors(colors) {
        for (int color = 0; color < 6; color++) {
            if (colors.count((enum colors)color) != 1) {
                throw std::invalid_argument("Map must contain one character for each color");
            }
        }
    }

    void color_printer::print_row(std::ostream& os, std::span<const colors, 3> row) const {
        for (auto pos : row) {
            os << "\033[48;2;" << _colors.at(pos)[0] << ';' << _colors.at(pos)[1] << ';' << _colors.at(pos)[2] << "m  ";
        }
        os << "\033[0m";
    }

    void color_printer::print_blank_row(std::ostream& os) const {
        os << "      ";
    }

    text_printer::text_printer(const rubik_cube& cube, std::string_view scheme, const std::map<colors, char>& symbols)
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
