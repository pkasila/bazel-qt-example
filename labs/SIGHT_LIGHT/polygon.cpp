#include "polygon.h"
#include <algorithm>
#include <limits>
#include <cmath>

Polygon::Polygon(const std::vector<QPointF>& vertices)
    : m_vertices(vertices) {}

const std::vector<QPointF>& Polygon::getVertices() const {
    return m_vertices;
}

void Polygon::addVertex(const QPointF& vertex) {
    m_vertices.push_back(vertex);
}

void Polygon::updateLastVertex(const QPointF& new_vertex) {
    if (!m_vertices.empty()) {
        m_vertices.back() = new_vertex;
    }
}

size_t Polygon::vertexCount() const {
    return m_vertices.size();
}
bool Polygon::isEmpty() const {
    return m_vertices.empty();
}

std::optional<QPointF> Polygon::intersectRay(const Ray& ray_to_intersect) const {
    if (m_vertices.size() < 2) {
        return std::nullopt;
    }

    const QPointF ray_origin = ray_to_intersect.getBegin();
    const QPointF ray_far_end = ray_to_intersect.getEnd();

    double min_hit_dist_sq = std::numeric_limits<double>::max();
    QPointF nearest_hit_point;
    bool intersection_occurred = false;

    for (size_t i = 0; i < m_vertices.size(); ++i) {
        const QPointF poly_v1 = m_vertices[i];
        const QPointF poly_v2 = m_vertices[(i + 1) % m_vertices.size()];

        auto current_hit_opt = segmentIntersection(ray_origin, ray_far_end, poly_v1, poly_v2);

        if (current_hit_opt) {
            QPointF diff_vec = *current_hit_opt - ray_origin;
            double hit_dist_sq = QPointF::dotProduct(diff_vec, diff_vec);

            if (hit_dist_sq > 1e-12 && hit_dist_sq < min_hit_dist_sq) {
                min_hit_dist_sq = hit_dist_sq;
                nearest_hit_point = *current_hit_opt;
                intersection_occurred = true;
            }
        }
    }

    return intersection_occurred ? std::optional(nearest_hit_point) : std::nullopt;
}

std::optional<QPointF> Polygon::segmentIntersection(
    const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4) const {
    double p1x = p1.x(), p1y = p1.y();
    double p2x = p2.x(), p2y = p2.y();
    double p3x = p3.x(), p3y = p3.y();
    double p4x = p4.x(), p4y = p4.y();

    double denominator = (p1x - p2x) * (p3y - p4y) - (p1y - p2y) * (p3x - p4x);

    if (std::abs(denominator) < 1e-9) {
        return std::nullopt;
    }

    double t_numerator = (p1x - p3x) * (p3y - p4y) - (p1y - p3y) * (p3x - p4x);
    double u_numerator = -((p1x - p2x) * (p1y - p3y) - (p1y - p2y) * (p1x - p3x));

    double t = t_numerator / denominator;
    double u = u_numerator / denominator;

    const double epsilon = 1e-9;
    if (t >= -epsilon && t <= 1.0 + epsilon &&
        u >= -epsilon && u <= 1.0 + epsilon) {
        return QPointF(p1x + t * (p2x - p1x), p1y + t * (p2y - p1y));
    }
    return std::nullopt;
}

