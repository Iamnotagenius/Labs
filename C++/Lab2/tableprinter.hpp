#include <array>
#include <iostream>
#include <ostream>

template<unsigned int Columns = 2>
class TablePrinter {
    public:
        // second argument in template definition
        // is simply "removes" functions with wrong number of arguments
        template<class... Ints, 
            typename std::enable_if<sizeof...(Ints) == Columns, int>::type = 0>
        TablePrinter(std::ostream& stream, Ints... widths) : 
            _out(stream), _widths{static_cast<int>(widths) ...} {
            static_assert(Columns != 0, "Number of columns must be... Something.");
            _out.setf(std::ios::left, std::ios::adjustfield);
        }
        template<class... Ints, 
            typename std::enable_if<sizeof...(Ints) == Columns, int>::type = 0>
        TablePrinter(Ints... widths) : TablePrinter(std::cout, widths...) {}

        template<class T, class... Args, 
            typename std::enable_if<sizeof...(Args) == Columns - 1, int>::type = 0>
        void operator()(T first, Args... rest) {
            _out.width(0);
            _out << '|';
            print_element(first, rest...);
            _out << '\n';
        }
        
        void operator()() {
            _out.fill('-');
            for (auto width : _widths) {
                _out.width(width + 3);
                _out << '+';
            }
            _out << '+' << '\n';
            _out.fill(' ');
        }
        
    private:
        std::ostream& _out;
        std::array<int, Columns> _widths;
        
        template<class T, class... Ts>
        void print_element(T item, Ts... rest) {
            _out << ' ';
            _out.width(_widths[Columns - sizeof...(rest) - 1] + 1);
            _out << item; 
            _out.width(0);
            _out << '|';
            if constexpr (sizeof...(rest) > 0)
                print_element(rest...);
        }
};
