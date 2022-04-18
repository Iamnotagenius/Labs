#ifndef GEOMETRY_HEADER
#define GEOMETRY_HEADER

#include <ostream>
#include <vector>
#include <initializer_list>

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

            friend std::ostream& operator<<(std::ostream& out, 
                                            Geometry::Point a);
    
            double GetX();
            double GetY();
        private:
            double _x, _y;
    };

    double GetDistance(Point a, Point b);
    void SortClockwise(std::vector<Point>& points);
    bool HasIntersection(std::vector<Point> points);
    
    class PolyLine {
        public:
            template<class TContainer>
            PolyLine(TContainer& points) :
                _points(std::vector<Point>(points)) {}
            PolyLine(const PolyLine& line);
            PolyLine(std::initializer_list<Point> points);

            PolyLine& operator=(PolyLine other);

            size_t GetVerticesCount();
            virtual double Perimeter();

        protected:
            std::vector<Point> _points;
    };

    class CycledPolyLine : public PolyLine {
        public:
            using PolyLine::PolyLine;
            using PolyLine::operator=;
            double Perimeter() final;
    };

    class Polygon : public CycledPolyLine {
        public:
            template<class TContainer>
            Polygon(TContainer& points);
            Polygon(std::initializer_list<Point> points);
            Polygon(const Polygon& polygon);
            using PolyLine::operator=;

            double Area();
    };

    class Triangle : public Polygon {
        public:
            Triangle(Point p1, Point p2, Point p3);
            Triangle(const Triangle& other);
            using PolyLine::operator=;

            bool IsIsosceles();
            bool IsEquilateral();
    };

    class Trapezoid : public Polygon {
        public:
            Trapezoid(Point first_p, Point second_p, double first_len, double second_len, double angle);
            Trapezoid(const Trapezoid& other);

            using PolyLine::operator=;

            bool IsIsosceles();
    };

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
