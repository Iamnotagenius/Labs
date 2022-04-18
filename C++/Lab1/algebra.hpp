#ifndef ALGEBRA_INCLUDED
#define ALGEBRA_INCLUDED

#include <initializer_list>
#include <vector>
#include <ostream>
#include <functional>

namespace Algebra {
    class Polynom {
        public:
            template<class TContainer>
            Polynom(const TContainer& coefs) : _coefs(new std::vector<int>(coefs)) {}
            Polynom(std::initializer_list<int> coefs);
            Polynom();
            Polynom(std::vector<int>* vec);
            template<class... Ints>
            Polynom(Ints... coefs) : _coefs(new std::vector<int>({static_cast<int>(coefs) ...})) {}
            Polynom(const Polynom& other);
    
            Polynom& operator=(Polynom other);
    
            ~Polynom();
    
            size_t Power() const;
    
            int operator[] (size_t index) const;
            int& operator[](size_t index);
    
            int operator()(int x);
    
            bool operator==(Polynom other);
            bool operator!=(Polynom other);
    
            Polynom& operator+=(const Polynom& other);
            friend Polynom operator+(const Polynom& lhs, const Polynom& rhs);
    
            Polynom& operator-=(const Polynom& other);
            friend Polynom operator-(const Polynom& lhs, const Polynom& rhs);
    
            Polynom operator-();
    
            Polynom& operator*=(int scalar);
            friend Polynom operator*(const Polynom& lhs, int rhs);
    
            Polynom& operator/=(int scalar);
            friend Polynom operator/(const Polynom& lhs, int rhs);
    
            Polynom& operator<<=(size_t pow);
            friend Polynom operator<<(const Polynom& lhs, size_t pow);
    
            Polynom& operator>>=(size_t pow);
            friend Polynom operator>>(const Polynom& lhs, int pow);

            friend std::ostream& operator<<(std::ostream& os, const Polynom& polynom);

        private:
            std::vector<int>* _coefs;
            Polynom& performBinaryOperation(Polynom other, std::function<int(int, int)> op);
    };
}
#endif
