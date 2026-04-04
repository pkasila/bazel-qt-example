#include "raycaster.h"

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : m_begin(begin), m_end(end), m_angle(angle) {
}

Ray Ray::Rotate(double delta) const {
    double newAngle = m_angle + delta;
    QPointF direction(std::cos(newAngle), std::sin(newAngle));
    return Ray(m_begin, m_begin + direction * 1000000.0, newAngle);
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (m_vertices.size() < 2) {
        return std::nullopt;
    }
    std::optional<QPointF> closestHit = std::nullopt;
    double minDistSq = 1e18;

    const QPointF r_start = ray.getBegin();
    const QPointF r_end = ray.getEnd();

    auto intersect = [](QPointF p1, QPointF p2, QPointF p3, QPointF p4) -> std::optional<QPointF> {
        double den = (p1.x() - p2.x()) * (p3.y() - p4.y()) - (p1.y() - p2.y()) * (p3.x() - p4.x());
        if (std::abs(den) < 1e-9) {
            return std::nullopt;
        }

        double t =
            ((p1.x() - p3.x()) * (p3.y() - p4.y()) - (p1.y() - p3.y()) * (p3.x() - p4.x())) / den;
        double u =
            -((p1.x() - p2.x()) * (p1.y() - p3.y()) - (p1.y() - p2.y()) * (p1.x() - p3.x())) / den;

        if (t > 1e-6 && u >= 0 && u <= 1) {
            return QPointF(p1.x() + t * (p2.x() - p1.x()), p1.y() + t * (p2.y() - p1.y()));
        }
        return std::nullopt;
    };

    for (size_t i = 0; i < m_vertices.size(); ++i) {
        auto hit =
            intersect(r_start, r_end, m_vertices[i], m_vertices[(i + 1) % m_vertices.size()]);
        if (hit) {
            double dx = hit->x() - r_start.x();
            double dy = hit->y() - r_start.y();
            double dSq = dx * dx + dy * dy;
            if (dSq < minDistSq) {
                minDistSq = dSq;
                closestHit = hit;
            }
        }
    }
    return closestHit;
}

bool Polygon::IsPointInside(const QPointF& p) const {
    if (m_vertices.size() < 3) {
        return false;
    }
    bool inside = false;
    for (size_t i = 0, j = m_vertices.size() - 1; i < m_vertices.size(); j = i++) {
        if (((m_vertices[i].y() > p.y()) != (m_vertices[j].y() > p.y())) &&
            (p.x() < (m_vertices[j].x() - m_vertices[i].x()) * (p.y() - m_vertices[i].y()) /
                             (m_vertices[j].y() - m_vertices[i].y()) +
                         m_vertices[i].x())) {
            inside = !inside;
        }
    }
    return inside;
}