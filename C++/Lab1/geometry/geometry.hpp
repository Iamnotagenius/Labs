#ifndef GEOMETRY_HEADER
#define GEOMETRY_HEADER

#include <vector>
#include <initializer_list>

#include "point.hpp"

namespace Geometry {
    double GetDistance(Point a, Point b);
    void SortClockwise(std::vector<Point>& points);
    bool HasIntersection(std::vector<Point> points);
}
#endif
