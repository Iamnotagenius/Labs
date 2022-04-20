#ifndef RUBIK_HPP
#define RUBIK_HPP

#include <string>
#include <istream>
#include <map>
#include <span>
#include <ostream>
#include <array>


namespace rubik {
    enum colors {
        RED, ORANGE,
        WHITE, YELLOW,
        BLUE, GREEN
    };
    
    enum sides {
        FRONT, RIGHT, BACK, LEFT, TOP, BOTTOM
    };
    

    std::istream& operator>>(std::istream&, colors&);

    using state_array = std::array<std::array<colors, 9>, 6>;

    
    class rubik_cube {
        private:
            state_array _state;
            void validate();
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

        public:
            struct miniside {
                sides side;
                colors color;
            };
            using edge = std::array<miniside, 2>;
            using corner = std::array<miniside, 3>;            // Constructs solved cube

            rubik_cube();
            // Constructs scrambled cube
            rubik_cube(int times);
            rubik_cube(const std::string& scramble);
            rubik_cube(const state_array& scramble);

            const state_array& get_state() const;
            std::array<edge, 12> get_edges() const;
            std::array<corner, 8> get_corners() const;
    
            // turns
    
            void F();
            void R();
            void U();
            void L();
            void B();
            void D();
            
            void F2();
            void R2();
            void U2();
            void L2();
            void B2();
            void D2();
        
            void Fi();
            void Ri();
            void Ui();
            void Li();
            void Bi();
            void Di();
    
            // Returns solution
            std::string solve();
    
    };

    class cube_printer {
        private:
            const state_array& _cube;
            const std::string& _scheme;
        public:
            cube_printer(const rubik_cube&, const std::string&);
            virtual void print_row(std::ostream&, std::span<const colors, 3>) const = 0;
            virtual void print_blank_row(std::ostream&) const = 0;
            friend std::ostream& operator<<(std::ostream&, const cube_printer&);
            
    };
    class color_printer : public cube_printer {
        private:
            const std::map<const colors, char> _symbols;
        public:
            color_printer(const rubik_cube&, const std::string&);
            color_printer(const rubik_cube&, const std::string&, const std::map<const colors, char>& symbols);
            void print_row(std::ostream&, std::span<const colors, 3>) const override;
            void print_blank_row(std::ostream&) const override;
    };
    class text_printer : public cube_printer {
        private:
            const std::map<const colors, char> _symbols;
        public:
            text_printer(const rubik_cube&, const std::string&);
            text_printer(const rubik_cube&, const std::string&, const std::map<const colors, char>& symbols);
            void print_row(std::ostream&, std::span<const colors, 3>) const override;
            void print_blank_row(std::ostream&) const override;
    };
}
std::ostream& operator<<(std::ostream&, rubik::sides);
#endif
