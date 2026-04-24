#include "polygon.h"
#include <cmath>

Polygon::Polygon(const std::vector<QPointF>& vertices) : m_vertices(vertices) {}
const std::vector<QPointF>& Polygon::getVertices() const { return m_vertices; }
void Polygon::AddVertex(const QPointF& vertex) { m_vertices.push_back(vertex); }
void Polygon::UpdateLastVertex(const QPointF& new_vertex) { if (!m_vertices.empty()) m_vertices.back() = new_vertex; }

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (m_vertices.size() < 2) return std::nullopt;
    QPointF p1 = ray.getBegin();
    QPointF p2 = ray.getEnd();
    std::optional<QPointF> closest;
    double minDistSq = 1e18;

    for (size_t i = 0; i < m_vertices.size(); ++i) {
        QPointF p3 = m_vertices[i];
        QPointF p4 = m_vertices[(i + 1) % m_vertices.size()];
        double den = (p1.x() - p2.x()) * (p3.y() - p4.y()) - (p1.y() - p2.y()) * (p3.x() - p4.x());
        if (std::abs(den) < 1e-9) continue;
        double t = ((p1.x() - p3.x()) * (p3.y() - p4.y()) - (p1.y() - p3.y()) * (p3.x() - p4.x())) / den;
        double u = -((p1.x() - p2.x()) * (p1.y() - p3.y()) - (p1.y() - p2.y()) * (p1.x() - p3.x())) / den;
        if (t > 0 && u >= 0 && u <= 1) {
            QPointF hit(p1.x() + t * (p2.x() - p1.x()), p1.y() + t * (p2.y() - p1.y()));
            double d = std::pow(hit.x() - p1.x(), 2) + std::pow(hit.y() - p1.y(), 2);
            if (d < minDistSq) { minDistSq = d; closest = hit; }
        }
    }
    return closest;
}