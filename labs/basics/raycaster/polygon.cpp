#include "polygon.h"
#include <limits>
#include <cmath>

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {}

std::vector<QPointF> Polygon::getVertices() const { return vertices_; }

void Polygon::addVertex(const QPointF& vertex) { vertices_.push_back(vertex); }

void Polygon::updateLastVertex(const QPointF& newVertex) {
    if (!vertices_.empty()) {
        vertices_.back() = newVertex;
    }
}

std::optional<QPointF> Polygon::intersectRay(const Ray& ray) const {
    if (vertices_.size() < 2) return std::nullopt;

    std::optional<QPointF> closestIntersection;
    double minDistance = std::numeric_limits<double>::max();

    const QPointF& rayBeg = ray.getBegin();
    const QPointF& rayEnd = ray.getEnd();

    for (size_t i = 0; i < vertices_.size(); ++i) {
        size_t j = (i + 1) % vertices_.size();
        const QPointF& a = vertices_[i];
        const QPointF& b = vertices_[j];
        auto intersection = segmentRayIntersection(a, b, rayBeg, rayEnd);

        if (intersection) {
            double dist = distanceSquared(rayBeg, *intersection);
            if (dist < minDistance) {
                minDistance = dist;
                closestIntersection = intersection;
            }
        }
    }

    return closestIntersection;
}

std::optional<QPointF> Polygon::segmentRayIntersection(const QPointF& A, const QPointF& B,
                                                       const QPointF& rayBeg, const QPointF& rayEnd) {
    double x1 = A.x(), y1 = A.y();
    double x2 = B.x(), y2 = B.y();
    double x3 = rayBeg.x(), y3 = rayBeg.y();
    double x4 = rayEnd.x(), y4 = rayEnd.y();

    double denom = (x1 - x2)*(y3 - y4) - (y1 - y2)*(x3 - x4);
    if (std::abs(denom) < 1e-10) return std::nullopt;

    double t = ((x1 - x3)*(y3 - y4) - (y1 - y3)*(x3 - x4)) / denom;
    double u = -((x1 - x2)*(y1 - y3) - (y1 - y2)*(x1 - x3)) / denom;

    if (t >= 0.0 && t <= 1.0 && u >= 0.0) {
        return QPointF(x1 + t * (x2 - x1), y1 + t * (y2 - y1));
    }

    return std::nullopt;
}

double Polygon::distanceSquared(const QPointF& a, const QPointF& b) {
    return std::pow(a.x() - b.x(), 2) + std::pow(a.y() - b.y(), 2);
}
