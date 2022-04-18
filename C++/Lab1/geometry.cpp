#include <cmath>
#include <functional>
#include <numeric>
#include <ostream>
#include <vector>
#include "geometry.hpp"

namespace Geometry {
    
    Point::Point(double x, double y) : _x(x), _y(y) {}
    Point::Point(double angle) : _x(cos(angle)), _y(sin(angle)) {}
    Point::Point(const Point& p) : _x(p._x), _y(p._y) {}

    Point& Point::operator=(Point other) {
                _x = other._x;
                _y = other._y;
                return *this;
            }
    
    Point Point::operator+(Point other) {
                return Point(_x + other._x, _y + other._y);
            }
    
    Point Point::operator-(Point other) {
                return Point(_x - other._x, _y - other._y);
            }
    
    Point Point::operator*(double scalar) {
                return Point(_x * scalar, _y * scalar);
            }
    
    Point Point::operator/(double scalar) {
                return Point(_x / scalar, _y / scalar);
            }
    
    double Point::GetX() {
        return _x;
    }
    double Point::GetY() {
        return _y;
    }

    std::ostream& operator<<(std::ostream& out, Geometry::Point a) {
        return out << '(' << a.GetX() << ", " << a.GetY() << ")";
    }

    double GetDistance(Point a, Point b) {
        return std::hypot(a.GetX() - b.GetX(), a.GetY() - b.GetY());
    }

    void SortClockwise(std::vector<Point>& points) {
        // Finding a center point as a mean of points
        Point center = [&points]() {
            return std::accumulate(
                points.begin(),
                points.end(), Point(0, 0)) / points.size();
        }();
        // Substracting center point
        std::for_each(points.begin(), points.end(), 
            std::bind(std::minus<void>(), std::placeholders::_1, center));
        // Sorting points by angle and distance
        std::sort(points.begin(), points.end(), 
            [](Point a, Point b) {
                auto getAngle = [](Point a) {
                    double angle = atan2(a.GetY(), a.GetX());
                    if (angle <= 0) 
                        angle += 2 * M_PI;
                    return angle;
                };
                if (getAngle(a) < getAngle(b))
                    return false;
                
                if (getAngle(a) == getAngle(b) && hypot(a.GetX(), a.GetY()) < hypot(b.GetX(), b.GetY()))
                    return false;

                return true;
            }
        );
        // Adding center point back
        std::for_each(points.begin(), points.end(), 
            std::bind(std::plus<void>(), std::placeholders::_1, center));
    }
    
    PolyLine::PolyLine(const PolyLine& line) : _points(line._points) {}
    
    PolyLine::PolyLine(std::initializer_list<Point> points) : _points(points) {}

    PolyLine& PolyLine::operator=(PolyLine other) {
        std::swap(_points, other._points);
        return *this;
    }
    
    size_t PolyLine::GetVerticesCount() {
        return _points.size();
    }

    double PolyLine::Perimeter() {
        auto current = _points.begin();
        current = std::next(current);
        double distance = 0;
        while (current != _points.end()) {
            distance += GetDistance(*current, *std::prev(current));
            current = std::next(current);
        }
        return distance;
    }
    
    double CycledPolyLine::Perimeter() {
        return PolyLine::Perimeter() +
                GetDistance(_points[0], _points[_points.size() - 1]);
    }

    template<class TContainer>
    Polygon::Polygon(TContainer& points) : CycledPolyLine(points) {
        SortClockwise(_points);
    }
    
    Polygon::Polygon(std::initializer_list<Point> points) : CycledPolyLine(points) {
        SortClockwise(_points);
    }

    Polygon::Polygon(const Polygon& polygon) : CycledPolyLine(polygon._points) {
        SortClockwise(_points);
    }


    double Polygon::Area() {
        double sum;
        size_t n = _points.size();
        for (size_t i = 0; i < n; ++i) {
            sum += _points[i].GetX() * _points[(i + 1) % n].GetY() -
                   _points[i].GetY() * _points[(i + 1) % n].GetX();
        }
        return std::abs(sum / 2);
    }
    
    Triangle::Triangle(Point p1, Point p2, Point p3) : Polygon({p1, p2, p3}) {}

    Triangle::Triangle(const Triangle& other) : Polygon(other._points) {}


    bool Triangle::IsIsosceles() {
        return GetDistance(_points[0], _points[1]) == GetDistance(_points[0], _points[2]) ||
               GetDistance(_points[1], _points[0]) == GetDistance(_points[1], _points[2]) ||
               GetDistance(_points[2], _points[0]) == GetDistance(_points[2], _points[1]);
    }

    bool Triangle::IsEquilateral() {
        return GetDistance(_points[0], _points[1]) ==
               GetDistance(_points[1], _points[2]) &&
               GetDistance(_points[1], _points[2]) ==
               GetDistance(_points[2], _points[0]);
    }

    Trapezoid::Trapezoid(Point first_p, Point second_p, double first_len, double second_len, double angle) : 
                Polygon(
                    {
                        first_p, 
                        second_p, 
                        second_p + Point(angle) * second_len, 
                        first_p + Point(angle) * first_len
                        }
                    ) {}
            
    Trapezoid::Trapezoid(const Trapezoid& other) : Polygon(other._points) {}
            

    bool Trapezoid::IsIsosceles() {
        return GetDistance(_points[0], _points[1]) ==
               GetDistance(_points[2], _points[3]);
    }

    RegularPolygon::RegularPolygon(Point center, Point first, int vertices) : Polygon({}) {
        double distance = GetDistance(center, first);
        Point initial = first - center;
        double angle = atan2(initial.GetY(), initial.GetX());
        if (angle <= 0)
            angle += 2 * M_PI;
        Construct(distance, angle, vertices);
        std::for_each(_points.begin(), _points.end(), 
            std::bind(std::plus<void>(), std::placeholders::_1, center));
    }

    RegularPolygon::RegularPolygon(Point center, double distance, double angle, int vertices) : Polygon({}) {
        Construct(distance, angle, vertices);
        std::for_each(_points.begin(), _points.end(), 
            std::bind(std::plus<void>(), std::placeholders::_1, center));
    }

    RegularPolygon::RegularPolygon(const RegularPolygon& other) : Polygon(other._points) {}
    
    void RegularPolygon::Construct(double distance, double angle, int vertices) {
        if (vertices < 3)
            throw std::out_of_range("Count of vertices must be greater or equal to 3");
         
        for (int i = 0; i < vertices; ++i) {
            _points.push_back(Point(angle) * distance);
            angle += 2 * M_PI / vertices;
        }
    }

}
