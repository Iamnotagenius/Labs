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
            struct miniside {
                sides side;
                colors color;
            };

            template<std::size_t N>
            struct minicube {
                std::array<miniside, N> sides;
                minicube(std::array<miniside, N> s) :
                    sides(s) {}
                bool has_color(enum colors color) {
                    return std::any_of(sides.cbegin(), sides.cend(), 
                            [color](miniside s) { return s.color == color; });
                }
                bool on_side(enum sides side) {
                    return std::any_of(sides.cbegin(), sides.cend(), 
                            [side](miniside s) { return s.side == side; });
                }
            };
            
        public:
            struct edge : public minicube<2> {
                using minicube<2>::minicube;
            };
            struct corner : public minicube<3> {
                using minicube<3>::minicube;
            };
            // Constructs solved cube
            rubik_cube();
            // Constructs scrambled cube
            rubik_cube(int times);
            rubik_cube(const std::string& scramble);
            rubik_cube(const std::array<int, 9*6>& scramble);

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
#endif
