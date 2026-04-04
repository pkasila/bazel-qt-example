#include "raycaster_polygon.h"
#include <limits>

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : m_begin(begin), m_end(end), m_angle(angle) {}

QPointF Ray::getBegin() const { return m_begin; }
void Ray::setBegin(const QPointF& begin) { m_begin = begin; }

QPointF Ray::getEnd() const { return m_end; }
void Ray::setEnd(const QPointF& end) { m_end = end; }

double Ray::getAngle() const { return m_angle; }
void Ray::setAngle(double angle) { m_angle = angle; }

Ray Ray::Rotate(double angle_rad) const {
    double dx = m_end.x() - m_begin.x();
    double dy = m_end.y() - m_begin.y();

    double new_dx = dx * std::cos(angle_rad) - dy * std::sin(angle_rad);
    double new_dy = dx * std::sin(angle_rad) + dy * std::cos(angle_rad);

    QPointF new_end(m_begin.x() + new_dx, m_begin.y() + new_dy);
    return Ray(m_begin, new_end, m_angle + angle_rad);
}

Polygon::Polygon(const std::vector<QPointF>& vertices) : m_vertices(vertices) {}

const std::vector<QPointF>& Polygon::getVertices() const { return m_vertices; }

void Polygon::AddVertex(const QPointF& vertex) {
    m_vertices.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (!m_vertices.empty()) {
        m_vertices.back() = new_vertex;
    }
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (m_vertices.size() < 2) return std::nullopt;

    std::optional<QPointF> closest_point = std::nullopt;
    double min_t1 = std::numeric_limits<double>::infinity();

    double r_px = ray.getBegin().x();
    double r_py = ray.getBegin().y();
    double r_dx = ray.getEnd().x() - r_px;
    double r_dy = ray.getEnd().y() - r_py;

    for (size_t i = 0; i < m_vertices.size(); ++i) {
        QPointF v1 = m_vertices[i];
        QPointF v2 = m_vertices[(i + 1) % m_vertices.size()];

        double s_px = v1.x();
        double s_py = v1.y();
        double s_dx = v2.x() - s_px;
        double s_dy = v2.y() - s_py;

        double det = r_dx * s_dy - r_dy * s_dx;
        
        if (std::abs(det) < 1e-7) continue;

        double t1 = ((s_px - r_px) * s_dy - (s_py - r_py) * s_dx) / det;
        double t2 = ((s_px - r_px) * r_dy - (s_py - r_py) * r_dx) / det;

        if (t1 >= 0 && t2 >= 0 && t2 <= 1.0) {
            if (t1 < min_t1) {
                min_t1 = t1;
                closest_point = QPointF(r_px + r_dx * t1, r_py + r_dy * t1);
            }
        }
    }

    return closest_point;
}

void Polygon::RemoveLastVertex() {
    if (m_vertices.size() > 1) {
        m_vertices.pop_back();
    }
}