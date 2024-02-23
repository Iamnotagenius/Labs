#include <cmath>
#include <functional>
#include <initializer_list>
#include <ostream>
#include <vector>
#include "algebra.hpp"

namespace Algebra {
    Polynom::Polynom(std::initializer_list<int> coefs) : _coefs(new std::vector<int>(coefs)) {}
    Polynom::Polynom() : _coefs(new std::vector<int>({0})) {}
    Polynom::Polynom(std::vector<int>* vec) : _coefs(vec) {}
    Polynom::Polynom(const Polynom& other) : _coefs(new std::vector<int>(*other._coefs)) {}
    Polynom& Polynom::operator=(Polynom other) {
        std::swap(_coefs, other._coefs);
        return *this;
    }

    Polynom::~Polynom() {
        delete _coefs;
    }

    size_t Polynom::Power() const { return _coefs->size() - 1; }


    int Polynom::operator[] (size_t index) const {            
        if (index >= _coefs->size()) 
            return 0;
        return (*_coefs)[index];
    }

    int& Polynom::operator[](size_t index) {
        if (index >= _coefs->size()) {
            _coefs->resize(index + 1);
        }
        return (*_coefs)[index];
    }
    
    int Polynom::operator()(int x) {
        int res = (*_coefs)[0];
        for (size_t i = 1; i <= Power(); ++i)
            res += std::pow(x, i) * (*_coefs)[i];
        return res;
    }

    bool Polynom::operator==(Polynom other) {
        if (Power() != other.Power()) {
            return false;
        }
        for (size_t i = 0; i < Power(); ++i) {
            if ((*this)[i] != other[i]) {
                return false;
            }
        }
        return true;
    }

    bool Polynom::operator!=(Polynom other) { return !(*this == other); }

    Polynom& Polynom::operator+=(const Polynom& other) {
        return performBinaryOperation(other, std::plus<>());
    }

    Polynom operator+(const Polynom& lhs, const Polynom& rhs) {
        auto result = Polynom(lhs);
        result += rhs;
        return result;
    }

    Polynom& Polynom::operator-=(const Polynom& other) {
        return performBinaryOperation(other, std::minus<>());
    }

    Polynom operator-(const Polynom& lhs, const Polynom& rhs) {
        auto result = Polynom(lhs);
        result -= rhs;
        return result;
    }

    Polynom Polynom::operator-() {
        auto result = new std::vector<int>(*_coefs);
        std::for_each(result->begin(), result->end(), [](auto& x){ x = -x; });
        return Polynom(result);
    }
    
    Polynom& Polynom::operator*=(int scalar) {
        std::for_each(_coefs->begin(), _coefs->end(), 
            [scalar](auto& coef) { coef *= scalar; });
        return *this;
    }

    Polynom operator*(const Polynom& lhs, int rhs) {
        auto result = Polynom(lhs);
        result *= rhs;
        return result;
    }
    
    Polynom& Polynom::operator/=(int scalar) {
        std::for_each(_coefs->begin(), _coefs->end(), 
            [scalar](auto& coef) { coef /= scalar; });
        return *this;
    }

    Polynom operator/(const Polynom& lhs, int rhs) {
        auto result = Polynom(lhs);
        result /= rhs;
        return result;
    }
    
    Polynom& Polynom::operator<<=(size_t pow) {
        for (size_t i = 0; i < pow; ++i) {
            _coefs->insert(_coefs->begin(), 0);
        }
        return *this;
    }

    Polynom operator<<(const Polynom& lhs, size_t pow) {
        auto result = Polynom(lhs);
        result <<= pow;
        return result;
    }

    Polynom& Polynom::operator>>=(size_t pow) {
        for (size_t i = 0; i < pow; ++i) {
            _coefs->erase(_coefs->begin());
        }
        return *this;
    }

    Polynom operator>>(const Polynom& lhs, int pow) {
        auto result = Polynom(lhs);
        result >>= pow;
        return result;
    }

    Polynom& Polynom::performBinaryOperation(Polynom other, std::function<int(int, int)> op) {
        auto min = Power() < other.Power() ? *this : other;
        for (size_t i = 0; i <= std::min(Power(), other.Power()); ++i) 
            _coefs->at(i) = op(_coefs->at(i), other._coefs->at(i));
        return *this;
    }

    std::ostream& operator<<(std::ostream& os, const Algebra::Polynom& polynom) {
        if (polynom[polynom.Power()] < 0)
            os << '-';
    
        int c;
        for (size_t i = polynom.Power(); i > 1; --i) {
            if (polynom[i] == 0)
                continue;
            c = std::abs(polynom[i]);
            if (c != 1)
                os << c;
            os << "x^" << i << (polynom[i - 1] < 0 ? " - " :  " + ");
        }
        c = std::abs(polynom[1]);
        if (c != 1)
            os << c;
    
        return os << "x" << (polynom[0] < 0 ? " - " :  " + ") << std::abs(polynom[0]);
    }
}

