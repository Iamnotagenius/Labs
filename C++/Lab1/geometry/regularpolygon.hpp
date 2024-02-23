#ifndef REGULARPOLYGON_HEADER
#define REGULARPOLYGON_HEADER

#include "polygon.hpp"
#include "polyline.hpp"

namespace Geometry {
    class RegularPolygon : public Polygon {
        public:
            RegularPolygon(Point center, Point first, int vertices);
            RegularPolygon(Point center, double distance, double angle, int vertices);
            RegularPolygon(const RegularPolygon& other);
            using PolyLine::operator=;
        private:
            void Construct(double distance, double angle, int vertices);
    };
}
#endif
