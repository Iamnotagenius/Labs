#ifndef CYCLEDPOLYLINE_HEADER
#define CYCLEDPOLYLINE_HEADER

#include "polyline.hpp"

namespace Geometry {
    class CycledPolyLine : public PolyLine {
        public:
            using PolyLine::PolyLine;
            using PolyLine::operator=;
            double Perimeter() final;
    };
}
#endif
