#include "polygon.h"

#include <limits>

namespace PolygonShapeNS {

PolygonShape::PolygonShape() = default;

PolygonShape::PolygonShape(const std::vector<QPoint>& points) : vertices(points) {
}

void PolygonShape::addVertex(const QPoint& pt) {
    vertices.push_back(pt);
}

void PolygonShape::updateLastVertex(const QPoint& pt) {
    if (!vertices.empty()) {
        vertices.back() = pt;
    }
}

void PolygonShape::clear() {
    vertices.clear();
}

const std::vector<QPoint>& PolygonShape::getVertices() const {
    return vertices;
}

bool PolygonShape::isValid() const {
    return vertices.size() >= 3;
}

std::vector<QPoint> PolygonShape::closedVertices() const {
    std::vector<QPoint> pts = vertices;
    if (!pts.empty()) {
        pts.push_back(pts.front());
    }
    return pts;
}

std::optional<QPoint> PolygonShape::findRayIntersection(const RaySegmentNS::RaySegment& ray) const {
    std::optional<QPoint> bestIntersection;
    double bestT = std::numeric_limits<double>::infinity();
    auto pts = closedVertices();
    for (size_t i = 1; i < pts.size(); ++i) {
        const QPoint& ptA = pts[i - 1];
        const QPoint& ptB = pts[i];
        double ray_dx = std::cos(ray.getDirection());
        double ray_dy = std::sin(ray.getDirection());
        auto optParams = computeIntersectionParams(ptA, ptB, ray.getStart(), ray_dx, ray_dy);
        if (!optParams.has_value()) {
            continue;
        }
        double denominator = ray_dx * (ptB.y() - ptA.y()) - ray_dy * (ptB.x() - ptA.x());
        double t = ((ptA.x() - ray.getStart().x()) * (ptB.y() - ptA.y()) -
                    (ptA.y() - ray.getStart().y()) * (ptB.x() - ptA.x())) /
                   denominator;
        double u =
            ((ptA.x() - ray.getStart().x()) * ray_dy - (ptA.y() - ray.getStart().y()) * ray_dx) /
            denominator;
        if (t >= 0 && u >= 0 && u <= 1) {
            if (t < bestT) {
                bestT = t;
                QPoint intersectPt(
                    static_cast<int>(ray.getStart().x() + ray_dx * t),
                    static_cast<int>(ray.getStart().y() + ray_dy * t));
                bestIntersection = intersectPt;
            }
        }
    }
    return bestIntersection;
}

}  // namespace PolygonShapeNS
