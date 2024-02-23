#ifndef POLYGON_HEADER
#define POLYGON_HEADER

#include "cycledpolyline.hpp"

namespace Geometry {
    class Polygon : public CycledPolyLine {
        public:
            template<class TContainer>
            Polygon(TContainer& points);
            Polygon(std::initializer_list<Point> points);
            Polygon(const Polygon& polygon);
            using PolyLine::operator=;

            double Area();
    };
}
#endif
