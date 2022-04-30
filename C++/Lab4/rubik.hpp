#ifndef RUBIK_HPP
#define RUBIK_HPP

#include <functional>
#include <string>
#include <istream>
#include <map>
#include <span>
#include <ostream>
#include <array>
#include <vector>


namespace rubik {
    enum colors {
        GREEN, RED, BLUE, ORANGE,
        WHITE, YELLOW,
    };
    
    enum sides {
        FRONT, RIGHT, BACK, LEFT, TOP, BOTTOM
    };
    

    std::istream& operator>>(std::istream&, colors&);

    using state_array = std::array<std::array<colors, 9>, 6>;

    
    class rubik_cube {
        private:
            state_array _state;
            // 0 1 2    6 3 0
            // 3 4 5 -> 7 4 1
            // 6 7 8    8 5 2
            static constexpr std::array<int, 9> rotate_clockwise_map = {
                6, 3, 0, 
                7, 4, 1,
                8, 5, 2
            };

            // 0 1 2    2 5 8
            // 3 4 5 -> 1 4 7
            // 6 7 8    0 3 6
            static constexpr std::array<int, 9> rotate_counterclockwise_map = {
                2, 5, 8, 
                1, 4, 7,
                0, 3, 6
            };
            
            // 0 1 2    8 7 6
            // 3 4 5 -> 5 4 3
            // 6 7 8    2 1 0
            static constexpr std::array<int, 9> rotate_twice_map = {
                8, 7, 6, 
                5, 4, 3,
                2, 1, 0
            };

            void rotate_side(sides, const std::array<int, 9>&);
            void rotate_axis(const std::array<sides, 6>&);

            std::map<std::size_t, std::function<void(std::string_view)>> _listeners;
            void call_listeners(std::string_view);

        public:
            struct miniside {
                sides side;
                colors color;
            };
            using edge = std::array<miniside, 2>;
            using corner = std::array<miniside, 3>;

            // Constructs solved cube
            rubik_cube();
            // Constructs scrambled cube
            rubik_cube(int times);
            rubik_cube(std::istream&);
            rubik_cube(std::istream&, const std::map<char, colors>&);
            rubik_cube(const state_array&);

            const state_array& get_state() const;
            std::array<edge, 12> get_edges() const;
            std::array<corner, 8> get_corners() const;
            std::map<colors, sides> get_centers() const;
            colors get_center(sides) const;
            edge get_edge(sides, sides) const;
            corner get_corner(sides, sides, sides) const;
            void validate();
    
            // turns
    
            rubik_cube& F();
            rubik_cube& R();
            rubik_cube& U();
            rubik_cube& L();
            rubik_cube& B();
            rubik_cube& D();
            
            rubik_cube& F2();
            rubik_cube& R2();
            rubik_cube& U2();
            rubik_cube& L2();
            rubik_cube& B2();
            rubik_cube& D2();
        
            rubik_cube& Fi();
            rubik_cube& Ri();
            rubik_cube& Ui();
            rubik_cube& Li();
            rubik_cube& Bi();
            rubik_cube& Di();

            rubik_cube& X();
            rubik_cube& Y();
            rubik_cube& Z();

            rubik_cube& Xi();
            rubik_cube& Yi();
            rubik_cube& Zi();


            rubik_cube& X2();
            rubik_cube& Y2();
            rubik_cube& Z2();

            rubik_cube& M();
            rubik_cube& E();
            rubik_cube& S();

            rubik_cube& Mi();
            rubik_cube& Ei();
            rubik_cube& Si();
    
            rubik_cube& M2();
            rubik_cube& E2();
            rubik_cube& S2();

            // Subscribing to a rotation event
            [[nodiscard("Returns an ID of a listener which then can be used to delete it")]] 
            std::size_t add_listener(std::function<void(std::string_view)>);
            void remove_listener(std::size_t);
            
    };

    void write_cube(std::ostream& os, const rubik_cube& cube, const std::map<colors, char> map);

    std::string old_pochmann(rubik_cube& cube);

    class cube_printer {
        private:
            const state_array& _cube;
            const std::string_view _scheme;
        public:
            cube_printer(const rubik_cube&);
            cube_printer(const rubik_cube&, std::string_view);
            virtual void print_row(std::ostream&, std::span<const colors, 3>) const = 0;
            virtual void print_blank_row(std::ostream&) const = 0;
            friend std::ostream& operator<<(std::ostream&, const cube_printer&);
    };
    class color_printer : public cube_printer {
        private:
            const std::map<colors, std::array<int, 3>> _colors;
        public:
            color_printer(const rubik_cube&);
            color_printer(const rubik_cube&, std::string_view);
            color_printer(const rubik_cube&, std::string_view, const std::map<colors, std::array<int, 3>>& symbols);
            color_printer(const rubik_cube&, const std::map<colors, std::array<int, 3>>& symbols);
            void print_row(std::ostream&, std::span<const colors, 3>) const override;
            void print_blank_row(std::ostream&) const override;
    };
    class text_printer : public cube_printer {
        private:
            const std::map<colors, char> _symbols;
        public:
            using cube_printer::cube_printer;
            text_printer(const rubik_cube&, std::string_view, const std::map<colors, char>& symbols);
            void print_row(std::ostream&, std::span<const colors, 3>) const override;
            void print_blank_row(std::ostream&) const override;
    };
}
std::ostream& operator<<(std::ostream&, rubik::sides);
std::ostream& operator<<(std::ostream& os, rubik::colors color);
#endif
