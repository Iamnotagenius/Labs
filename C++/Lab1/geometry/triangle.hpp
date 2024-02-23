#ifndef TRIANGLE_HEADER
#define TRIANGLE_HEADER

#include "polygon.hpp"
#include "polyline.hpp"

namespace Geometry {
    class Triangle : public Polygon {
        public:
            Triangle(Point p1, Point p2, Point p3);
            Triangle(const Triangle& other);
            using PolyLine::operator=;

            bool IsIsosceles();
            bool IsEquilateral();
    };
}
#endif
