#ifdef DEBUG
#include <iostream>
#endif
#include <cmath>
#include <functional>
#include <numeric>
#include <ostream>
#include <stdexcept>
#include <vector>
#include <set>
#include "point.hpp"
#include "polyline.hpp"
#include "cycledpolyline.hpp"
#include "polygon.hpp"
#include "triangle.hpp"
#include "trapezoid.hpp"
#include "regularpolygon.hpp"

#define EPSILON 0.0000001

namespace Geometry {
    
    Point::Point(double x, double y) : x(x), y(y) {}
    Point::Point(double angle) : x(cos(angle)), y(sin(angle)) {}
    Point::Point(const Point& p) : x(p.x), y(p.y) {}

    Point& Point::operator=(Point other) {
        x = other.x;
        y = other.y;
        return *this;
    }
    
    Point Point::operator+(Point other) {
        return Point(x + other.x, y + other.y);
    }
    
    Point Point::operator-(Point other) {
        return Point(x - other.x, y - other.y);
    }
    
    Point Point::operator*(double scalar) {
        return Point(x * scalar, y * scalar);
    }
    
    Point Point::operator/(double scalar) {
        return Point(x / scalar, y / scalar);
    }

    bool Point::operator==(Point other) const {
        return std::abs(x - other.x) < EPSILON && std::abs(y - other.y) < EPSILON;
    }
    
    std::ostream& operator<<(std::ostream& out, Geometry::Point a) {
        return out << '(' << a.x << ", " << a.y << ")";
    }

    double GetDistance(Point a, Point b) {
        return std::hypot(a.x - b.x, a.y - b.y);
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
                    double angle = atan2(a.y, a.x);
                    if (angle <= 0) 
                        angle += 2 * M_PI;
                    return angle;
                };
                if (getAngle(a) < getAngle(b))
                    return false;
                
                if (getAngle(a) == getAngle(b) && hypot(a.x, a.y) < hypot(b.x, b.y))
                    return false;

                return true;
            }
        );
        // Adding center point back
        std::for_each(points.begin(), points.end(), 
            std::bind(std::plus<void>(), std::placeholders::_1, center));
    }

    bool HasIntersection(std::vector<Point> points) {
        // sweep line algorithm used
        struct Segment { 
            Point left, right; 
            Segment(Point p1, Point p2) : left(p1), right(p2) {
                if (left.x > right.x) {
                    std::swap(left, right);
                }
            }
            #ifdef DEBUG
            void print() const {
                std::cerr << "[" << left << " -> " << right << "]";
            }
            #endif
        };
        struct Event {
            Point coordinate;
            Segment segment;
            Event(Point p, Segment& seg) : coordinate(p), segment(seg) {}
            bool isLeft() { return coordinate == segment.left; }
            bool operator<(const Event& other) const {
                if (std::abs(coordinate.y - other.coordinate.y) < EPSILON) {
                    return coordinate.x < other.coordinate.x;
               }
                return coordinate.y < other.coordinate.y;
            }
            bool operator==(Event& other) const {
                return coordinate == other.coordinate &&
                       segment.left == other.segment.left &&
                       segment.right == other.segment.right;
            }
            #ifdef DEBUG
            void print() const {
                std::cerr << coordinate << " "; segment.print();

            }
            #endif
        };
        auto isIntersecting = [](Segment s1, Segment s2) -> bool {
            auto orientation = [](Point p, Point q, Point r) {
                return (r.y - p.y) * (q.x - p.x) >
                       (q.y - p.y) * (r.x - p.x);
            };

            auto onSegment = [](Segment s, Point p) -> bool {
                return p.x < std::max(s.left.x, s.right.x) &&
                       p.x > std::min(s.left.x, s.right.x) &&
                       p.y < std::max(s.left.y, s.right.y) &&
                       p.y > std::min(s.left.y, s.right.y);
            };
            #ifdef DEBUG
            std::cerr << "Checking "; s1.print(); std::cerr << " and "; s2.print(); std::cerr << "\n";
            #endif
            // adjascent segments are not intersecting
            if (s1.left == s2.left || s1.left == s2.right || s1.right == s2.left || s1.right == s2.right)
                return false;
            
            std::vector<bool> orientations{
                orientation(s1.left, s1.right, s2.left),
                orientation(s1.left, s1.right, s2.right),
                orientation(s2.left, s2.right, s1.right),
                orientation(s2.left, s2.right, s1.left)
            };
            #ifdef DEBUG
            std::cerr << "Orientations: ";
            for (auto o : orientations) {
                std::cerr << o << " ";
            }
            std::cerr << "\n";
            #endif

            if (orientations[0] != orientations[1] &&
                orientations[2] != orientations[3]) {
                return true;
            }

            if (orientations[0] == 0 && onSegment(s1, s2.left)) {
                return true;
            }

            if (orientations[1] == 0 && onSegment(s1, s2.right)) {
                return true;
            }

            if (orientations[2] == 0 && onSegment(s2, s1.left)) {
                return true;
            }

            if (orientations[3] == 0 && onSegment(s2, s1.right)) {
                return true;
            }

            return false;
        };

        std::vector<Segment> segments;
        for (auto i = 0; i < points.size(); ++i) {
            segments.push_back(Segment(points[i], points[(i+1) % points.size()]));
        }

        std::vector<Event> events;
        
        for (auto seg : segments) {
            events.push_back(Event(seg.left, seg));
            events.push_back(Event(seg.right, seg));
        }
        
        std::sort(events.begin(), events.end(), [](Event& e1, Event& e2) { return e1.coordinate.x < e2.coordinate.x; });

        std::set<Event> active;

        for (auto event : events) {
                #ifdef DEBUG
                std::cerr << "Handling "; event.print(); std::cerr << "\n";
                #endif
            if (event.isLeft()) {
                auto next = active.lower_bound(event);
                auto prev = next == active.begin() ? active.end() : --next;
                
                if (next != active.end() && isIntersecting(event.segment, next->segment))
                    return true;

                if (prev != active.end() && isIntersecting(event.segment, prev->segment))
                    return true;

                #ifdef DEBUG
                std::cerr << "Inserting "; event.print(); std::cerr << "\n";
                #endif

                active.insert(event);
            }
            else {
                auto same = active.end();
                for (auto it = active.begin(); it != active.end(); ++it) {
                    if (it->segment.left == event.segment.left && it->segment.right == event.segment.right) {
                        same = it;
                        break;
                    }
                }
                #ifdef DEBUG
                std::cerr << "Found "; same->print(); std::cerr << "\n";
                #endif
                auto next(same);
                if (next != active.end()) {
                    ++next;
                }
                auto prev(same);
                prev = prev == active.begin() ? active.end() : --prev;
                
                if (next != active.end() && prev != active.begin() &&
                        isIntersecting(next->segment, prev->segment))
                    return true;

                if (same != active.end()) {
                    #ifdef DEBUG
                    std::cerr << "Erasing "; same->print(); std::cerr << "\n";
                    #endif
                    active.erase(same);
                }
            }

        }
        return false; 
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
        if (HasIntersection(points)) {
            throw std::logic_error("Ordered set of points has self intersections.");
        }
    }
    
    Polygon::Polygon(std::initializer_list<Point> points) : CycledPolyLine(points) {
        if (HasIntersection(points)) {
            throw std::logic_error("Ordered set of points has self intersections.");
        }
    }

    Polygon::Polygon(const Polygon& polygon) : CycledPolyLine(polygon._points) {}


    double Polygon::Area() {
        double sum = 0.0;
        size_t n = _points.size();
        for (size_t i = 0; i < n; ++i) {
            sum += _points[i].x * _points[(i + 1) % n].y -
                   _points[i].y * _points[(i + 1) % n].x;
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
        double angle = atan2(initial.y, initial.x);
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
