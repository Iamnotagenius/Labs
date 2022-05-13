#include <ostream>

template<int N, int P>
struct power {
    private:
        template<int Val, int Pow, int Res>
        struct iter {
            static constexpr int value = std::conditional_t<Pow & 1, iter<Val * Val, Pow / 2, Res * Val>, 
                                                                     iter<Val * Val, Pow / 2, Res>>::value;
        };
        template<int Val, int Res>
        struct iter<Val, 0, Res> {
            static constexpr int value = Res;
        };
    public:
        static constexpr int value = iter<N, P, 1>::value;
};

template<int... Coefs>
struct polynom {
    private:
        template<int X, int P, int Coef, int... Rem>
        struct iter {
            static constexpr int value = power<X, P>::value * Coef + iter<X, Rem...>::value;
        };

    
        template<int X, int P, int Coef>
        struct iter<X, P, Coef> {
            static constexpr int value = power<X, P>::value * Coef;
        };

        template<int X, int Coef>
        struct iter<X, 0, Coef> {
            static constexpr int value = Coef;
        };
        

    public:
        template<int X>
        struct eval {
            static constexpr int value = iter<X, Coefs...>::value;
        };
};

template<int... Coefs>
void to_str(std::ostream&, polynom<Coefs...>);

template<int P, int A, int... Coefs>
void to_str(std::ostream& os, polynom<P, A, Coefs...>) {
    os << (A < 0 ? " - " : " + ");
    if (A != 1 && A != -1) {
        os << std::abs(A);
    }
    if (P > 0) {
        os << "x";
        if (P > 1) {
            os << "^" << P;
        }
    }

    to_str<Coefs...>(os, polynom<Coefs...>());
}

template<>
void to_str<>(std::ostream &os, polynom<>) {}

template<int P, int A, int... Coefs>
std::ostream& operator<<(std::ostream& os, polynom<P, A, Coefs...> p) {
    if (A < 0) {
        os << '-';
    }
    if (std::abs(A) != 1) {
        os << A;
    }
    if (P > 0) {
        os << 'x';
        if (P > 1) {
            os << '^' << P;
        }
    }
    to_str<Coefs...>(os, polynom<Coefs...>());
    return os;
}
