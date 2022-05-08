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
            static constexpr int value = power<X, P>::value * Coef + iter<X, P + 1, Rem...>::value;
        };
    
        template<int X, int P, int Coef>
        struct iter<X, P, Coef> {
            static constexpr int value = power<X, P>::value * Coef;
        };

    public:
        template<int X>
        struct eval {
            static constexpr int value = iter<X, 0, Coefs...>::value;
        };
};

template<int P, int A, int... Coefs>
void to_str(std::ostream& os, polynom<A, Coefs...>) {
    os << (A < 0 ? " - " : " + ");
    if (A != 1 && A != -1) {
        os << std::abs(A);
    }
    os << "x";
    if (P > 1) {
        os << "^" << P;
    }
    to_str<P + 1, Coefs...>(os, polynom<Coefs...>());
}

template<int P, int A, int... Coefs>
void to_str(std::ostream& os, polynom<0, Coefs...>) {
    to_str<P + 1, Coefs...>(os, polynom<Coefs...>());
}

template<int P, int A, int... Coefs>
void to_str(std::ostream& os, polynom<0, A>) {
    os << (A < 0 ? " - " : " + ");
    if (A != 1 && A != -1) {
        os << std::abs(A);
    }
    os << "x^" << P;
}

template<int P>
void to_str(std::ostream& os, polynom<>) {}

template<int A, int... Coefs>
std::ostream& operator<<(std::ostream& os, polynom<A, Coefs...> p) {
    os << (A < 0 ? "-" : "") << A;
    to_str<1, Coefs...>(os, polynom<Coefs...>());
    return os;
}
