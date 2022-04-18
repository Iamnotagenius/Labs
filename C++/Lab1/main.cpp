#include <iostream>
#include "geometry.hpp"
#include "algebra.hpp"

int main() {
    using namespace Geometry;
    auto p1 = Point(1, 1);
    auto p2 = Point(2, 2);
    auto p3 = Point(3, 2);
    p2 = p1;
    std::cout << p2 << "\n";
    std::cout << p1 << "\n";
    p2 = p3;
    std::cout << p2 << "\n";
    std::cout << p1 << "\n";

    auto ps = {Point(1, 1), Point(1, 2), Point(2, 2)};
    auto v = std::vector<Point>({Point(0, 0), Point(1, 1)});
    auto vec = std::vector<Point>();
    vec.push_back(Point(0, 0));
    vec.push_back(Point(3, 0));
    vec.push_back(Point(0, 4));
    auto lines = std::vector<PolyLine*> { 
        new PolyLine(ps), 
        new CycledPolyLine(v),
        new PolyLine(vec)
    };
    
    for (auto line : lines) {
        std::cout << line->Perimeter() << std::endl;
    }
    auto polygons = std::vector<Polygon*> {
        new Triangle(vec[0], vec[1], vec[2]),
        new RegularPolygon(Point(0, 0), Point(0, 1), 3),
        new Trapezoid(Point(-1, 1), Point(-2, 0), 2, 4, 0),
        new Polygon(std::initializer_list<Point>{Point(1, 1), Point(-1, -1), Point(-1, 1), Point(1, -1)})
    };

    for (auto poly : polygons)
        std::cout << "Perimeter: " << poly->Perimeter() << "\nArea: " << poly->Area() << "\n";
    using namespace Algebra;
    auto t = Polynom(-1, -3, -4, 1);
    auto t2 = Polynom(3, 4, 5);
    std::cout << t2 << " where x = 3 is " << t2(3) << std::endl;

    std::cout << t << std::endl;
    return 0;
}
