#ifndef POLYGON_HPP
#define POLYGON_HPP

#include <QPointF>
#include "ray.hpp"

namespace rc {
    class Polygon;
}

class rc::Polygon {
public:
    void add(QPointF vertex);
    void setLast(QPointF vertex);
    void pop();
    [[nodiscard]] int size() const;
    [[nodiscard]] QPointF at(int index) const;
    [[nodiscard]] std::optional<QPointF> intersect(const Ray& ray) const;

private:
    std::vector<QPointF> vertices;
};

#endif // POLYGON_HPP
