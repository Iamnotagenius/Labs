#include <iostream>
#include <sstream>
#include <type_traits>
#include <array>
#include <gtest/gtest.h>
#include "constexpr_polynom.hpp"

enum { compile_time = polynom<0, 1, 1, 1, 2, 3>::eval<2>::value };

typedef polynom<0, 34, 
                1, -1, 
                2, -5, 
                3, 62, 
                5, 43> poly;

typedef polynom<2, 1, 
                1, -4, 
                0, 3> quadratic;

TEST(PolynomTest, CheckComputation) {
    EXPECT_EQ(poly::eval<5>::value, 142020);
    EXPECT_EQ(poly::eval<0>::value, 34);
    EXPECT_EQ(quadratic::eval<1>::value, 0);
    EXPECT_EQ(quadratic::eval<3>::value, 0);
    EXPECT_EQ(compile_time, 15);
}

TEST(PolynomTest, CheckOutput) {
    std::ostringstream output;
    output << poly();
    EXPECT_EQ(output.str(), "34 - x - 5x^2 + 62x^3 + 43x^5");
    output.str("");
    output << quadratic();
    EXPECT_EQ(output.str(), "x^2 - 4x + 3");
    output.str("");
    output << polynom<100, 2, 4, 6, 0, 5>();
    EXPECT_EQ(output.str(), "2x^100 + 6x^4 + 5");
}

