#ifndef TRAPEZOID_HEADER
#define TRAPEZOID_HEADER

#include "polygon.hpp"

namespace Geometry {
    class Trapezoid : public Polygon {
        public:
            Trapezoid(Point first_p, Point second_p, double first_len, double second_len, double angle);
            Trapezoid(const Trapezoid& other);

            using PolyLine::operator=;

            bool IsIsosceles();
    };

}
#endif
