#include "polygon.hpp"
#include <QPointF>
#include <QVector2D>
#include "util.hpp"

void rc::Polygon::add(QPointF vertex) { vertices.push_back(vertex); }
void rc::Polygon::setLast(QPointF vertex) { vertices.back() = vertex; }
void rc::Polygon::pop() { vertices.pop_back(); }
int rc::Polygon::size() const { return static_cast<int>(vertices.size()); }
QPointF rc::Polygon::at(int index) const { return vertices.at(index); }

std::optional<QPointF> rc::Polygon::intersect(const Ray& ray) const {
    QPointF rayBegin = ray.getOrigin();
    QPointF rayEnd = (QVector2D(ray.getOrigin()) + ray.getDirection() * 1e8).toPointF();
    std::optional<QPointF> res;

    for(int i = 0; i < vertices.size(); i++) {
        QPointF cur = vertices.at(i);
        QPointF next = vertices.at((i + 1) % vertices.size());
        std::optional<QPointF> pt = intersectSegments(cur, next, rayBegin, rayEnd);
        if(!pt.has_value()) {
            continue;
        }
        if(!res.has_value() || distance(rayBegin, pt.value()) < distance(rayBegin, res.value())) {
            res = pt;
        }
    }

    return res;
}
