#ifndef POINT_HEADER
#define POINT_HEADER

#include <ostream>

namespace Geometry {
    class Point {
        public:
            Point(double x, double y);
            Point(double angle);
            Point(const Point& p);
    
            Point& operator=(Point other);
            Point operator+(Point other);
            Point operator-(Point other);
            Point operator*(double scalar);
            Point operator/(double scalar);
            bool operator==(Point other) const;
    
            friend std::ostream& operator<<(std::ostream& out, 
                                            Geometry::Point a);
            double x, y;
    };
}
#endif
