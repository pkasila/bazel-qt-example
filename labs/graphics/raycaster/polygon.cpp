#include "polygon.h"
#include "ray.h"
#include <limits>

Polygon::Polygon(const std::vector<QPointF>& vertices) : m_vertices(vertices) {}

std::vector<QPointF> Polygon::getVertices() const { return m_vertices; }

void Polygon::addVertex(const QPointF& vertex) { m_vertices.push_back(vertex); }

void Polygon::updateLastVertex(const QPointF& new_vertex) {
    if (!m_vertices.empty()) m_vertices.back() = new_vertex;
}


std::optional<QPointF> Polygon::intersectRay(const Ray& ray) const {
    if (m_vertices.size() <= 1) {
        return std::nullopt;
    }
    double mn = 2e9;
    const double ep = 1e-9;
    QPointF begin1 = ray.getBegin();
    QPointF vec1 = ray.getEnd() - ray.getBegin();
    for (size_t i = 0; i < m_vertices.size(); i++) {
        QPointF begin2 = m_vertices[i];
        QPointF end2 = m_vertices[(i + 1) % m_vertices.size()];
        QPointF vec2 = end2 - begin2;
        double numer = vec2.y() * (begin2.x() - begin1.x()) - vec2.x() * (begin2.y() - begin1.y());
        double denomen = vec1.x() * vec2.y() - vec1.y() * vec2.x();
        double s1;
        double s2;
        if (std::abs(denomen) < ep) {
            continue;
        }
        s1 = numer / denomen;
        if (vec2.y() != 0.0) {
            s2 = (begin1.y() + vec1.y() * s1 - begin2.y()) / vec2.y();
        }
        else {
            s2 = (begin1.x() + vec1.x() * s1 - begin2.x()) / vec2.x();
        }
        if (s1 < -ep ||s2 < -ep || s2 > 1.0 + ep) {
            continue;
        }
        mn = std::min(mn, s1);
    }
    if (mn == 2e9) {
        return std::nullopt;
    }
    return std::optional<QPointF>(QPointF(begin1.x() + mn * vec1.x(), begin1.y() + mn * vec1.y()));
}

bool Polygon::isPointOnSegment(const QPointF& p, const QPointF& a, const QPointF& b) const {
    int cross = (p.x() - a.x()) * (b.y() - a.y()) - (p.y() - a.y()) * (b.x() - a.x());
    if (cross != 0) return false;

    int dot = QPointF::dotProduct(p - a, b - a);
    if (dot < 0) return false;

    int squaredLength = QPointF::dotProduct(b - a, b - a);
    if (dot > squaredLength) return false;

    return true;
}

bool Polygon::containsPoint(const QPointF& point) const {
    if (m_vertices.size() < 3) return false;

    bool inside = false;
    for (size_t i = 0, j = m_vertices.size() - 1; i < m_vertices.size(); j = i++) {
        const QPointF& p1 = m_vertices[i];
        const QPointF& p2 = m_vertices[j];

        if (((p1.y() > point.y()) != (p2.y() > point.y())) &&
            (point.x() < (p2.x() - p1.x()) * (point.y() - p1.y()) / (p2.y() - p1.y()) + p1.x())) {
            inside = !inside;
        }
    }
    return inside;
}

std::optional<QPointF> Polygon::lineIntersection(const QPointF& a1, const QPointF& a2,
                                                const QPointF& b1, const QPointF& b2) const {
    QPointF a = a2 - a1;
    QPointF b = b2 - b1;
    QPointF c = b1 - a1;

    int denominator = a.x() * b.y() - a.y() * b.x();
    if (denominator == 0) return std::nullopt;

    double sNumerator = c.x() * b.y() - c.y() * b.x();
    double tNumerator = c.x() * a.y() - c.y() * a.x();

    double s = sNumerator / denominator;
    double t = tNumerator / denominator;

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
        return a1 + QPointF(s * a.x(), s * a.y());
    }
    return std::nullopt;
}
