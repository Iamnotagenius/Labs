#include <ostream>

template<int N, int P>
struct power {
    static constexpr int value = power<N, P - 1>::value * N;
};
template<int N>
struct power<N, 0> {
    static constexpr int value = 1;
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
