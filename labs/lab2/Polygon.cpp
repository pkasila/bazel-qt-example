#include "Polygon.h"
#include <cmath>

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (m_vertices.size() < 2) return std::nullopt;
    std::optional<QPointF> closest = std::nullopt;
    double min_d = 1e18;
    QPointF p1 = ray.getBegin(), p2 = ray.getEnd();

    for (size_t i = 0; i < m_vertices.size(); ++i) {
        QPointF p3 = m_vertices[i], p4 = m_vertices[(i + 1) % m_vertices.size()];
        double den = (p1.x() - p2.x()) * (p3.y() - p4.y()) - (p1.y() - p2.y()) * (p3.x() - p4.x());
        if (std::abs(den) < 1e-9) continue;
        double t = ((p1.x() - p3.x()) * (p3.y() - p4.y()) - (p1.y() - p3.y()) * (p3.x() - p4.x())) / den;
        double u = -((p1.x() - p2.x()) * (p1.y() - p3.y()) - (p1.y() - p2.y()) * (p1.x() - p3.x())) / den;
        if (t > 0 && u >= 0 && u <= 1) {
            QPointF hit(p1.x() + t * (p2.x() - p1.x()), p1.y() + t * (p2.y() - p1.y()));
            double d = std::hypot(hit.x() - p1.x(), hit.y() - p1.y());
            if (d < min_d) { min_d = d; closest = hit; }
        }
    }
    return closest;
}