#ifndef POLYLINE_HEADER
#define POLYLINE_HEADER

#include <vector>
namespace Geometry {
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
}
#endif
