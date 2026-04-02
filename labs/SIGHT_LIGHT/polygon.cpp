#include "polygon.h"
#include <algorithm>
#include <limits>
#include <cmath>

Polygon::Polygon(const std::vector<QPointF>& vertices)
    : m_vertices(vertices)
{
}

const std::vector<QPointF>& Polygon::getVertices() const
{
    return m_vertices;
}

void Polygon::addVertex(const QPointF& vertex)
{
    m_vertices.push_back(vertex);
}

void Polygon::updateLastVertex(const QPointF& new_vertex)
{
    if (!m_vertices.empty()) {
        m_vertices.back() = new_vertex;
    }
}


// std::optional<QPointF> Polygon::intersectRay(const Ray& ray) const {
//     if (m_vertices.size() <= 1) {
//         return std::nullopt;
//     }
//     double mn = 2e9;
//     const double kEps = 1e-9;

//     QPointF begin1 = ray.getBegin();
//     QPointF dir1 = ray.getEnd() - ray.getBegin();
//     for (size_t i = 0; i < m_vertices.size(); i++) {
//         QPointF begin2 = m_vertices[i];
//         QPointF end2 = m_vertices[(i + 1) % m_vertices.size()];
//         QPointF dir2 = end2 - begin2;
//         double num = dir2.y() * (begin2.x() - begin1.x()) - dir2.x() * (begin2.y() - begin1.y());
//         double denom = dir1.x() * dir2.y() - dir1.y() * dir2.x();
//         double t1;
//         double t2;
//         if (std::abs(denom) < kEps) {
//             continue;
//         }
//         t1 = num / denom;
//         if (dir2.y() != 0.0) {
//             t2 = (begin1.y() + dir1.y() * t1 - begin2.y()) / dir2.y();
//         } else {
//             t2 = (begin1.x() + dir1.x() * t1 - begin2.x()) / dir2.x();
//         }
//         if (t1 < 0.0 || t2 < 0.0 || t2 > 1.0) {
//             continue;
//         }
//         QPointF new_point1 = begin1 + t1*dir1;

//         QPointF new_point2 = begin2 + t2*dir2;
//         if (std::abs(new_point1.x()-new_point2.x()) < std::numeric_limits<double>::epsilon())
//         mn = std::min(mn, t1);
//     }
//     if (mn == 2e9) {
//         return std::nullopt;
//     }
//     return std::optional<QPointF>(QPointF(begin1.x() + mn * dir1.x(), begin1.y() + mn * dir1.y()));
// }


double Polygon::Distance(const QPointF& p1, const QPointF& p2) {
    const double dx = p1.x() - p2.x();
    const double dy = p1.y() - p2.y();
    return std::sqrt((dx * dx) + (dy * dy));
}

std::optional<QPointF> Polygon::intersectRay(const Ray& ray) const {
    const QPointF ray_begin = ray.getBegin();
    const QPointF ray_end = ray.getEnd();

    double min = std::numeric_limits<double>::max();
    QPointF closest_intersection;
    bool found = false;

    for (size_t i = 0; i < m_vertices.size(); ++i) {
        const QPointF p1 = m_vertices[i];
        const QPointF p2 = m_vertices[(i + 1) % m_vertices.size()];

        auto intersection = lineIntersection(ray_begin, ray_end, p1, p2);

        if (intersection) {
            const double dist = Distance(ray_begin, *intersection);
            if (dist < min) {
                min = dist;
                closest_intersection = *intersection;
                found = true;
            }
        }
    }
    return found ? std::optional(closest_intersection) : std::nullopt;
}


// std::optional<QPoint> Polygon::lineIntersection(const QPoint& p1, const QPoint& p2,
//                                                 const QPoint& p3, const QPoint& p4) const
// {
//     double a1 = p2.y() - p1.y();
//     double b1 = p1.x() - p2.x();
//     double c1 = a1 * p1.x() + b1 * p1.y();

//     double a2 = p4.y() - p3.y();
//     double b2 = p3.x() - p4.x();
//     double c2 = a2 * p3.x() + b2 * p3.y();

//     double determinant = a1 * b2 - a2 * b1;

//     if (determinant == 0) {
//         return std::nullopt;
//     }

//     double x = (b2 * c1 - b1 * c2) / determinant;
//     double y = (a1 * c2 - a2 * c1) / determinant;

//     QPoint intersection(static_cast<int>(x), static_cast<int>(y));

//     if (pointOnSegment(intersection, p1, p2) && pointOnSegment(intersection, p3, p4)) {
//         return intersection;
//     }

//     return std::nullopt;
// }

std::optional<QPointF> Polygon::lineIntersection(
    const QPointF& a1, const QPointF& a2, const QPointF& b1, const QPointF& b2) const {
    const double denom =
        (a1.x() - a2.x()) * (b1.y() - b2.y()) - (a1.y() - a2.y()) * (b1.x() - b2.x());

    if (std::abs(denom) < std::numeric_limits<double>::epsilon()) {
        return std::nullopt;
    }

    const double t =
        ((a1.x() - b1.x()) * (b1.y() - b2.y()) - (a1.y() - b1.y()) * (b1.x() - b2.x())) / denom;

    const double u =
        -((a1.x() - a2.x()) * (a1.y() - b1.y()) - (a1.y() - a2.y()) * (a1.x() - b1.x())) / denom;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        return QPointF(a1.x() + t * (a2.x() - a1.x()), a1.y() + t * (a2.y() - a1.y()));
    }
    return std::nullopt;
}

bool Polygon::pointOnSegment(const QPointF& p, const QPointF& a, const QPointF& b) const
{
    int cross = (p.x() - a.x()) * (b.y() - a.y()) - (p.y() - a.y()) * (b.x() - a.x());
    if (abs(cross) > 1e-8) return false;
    bool x_ok = (p.x() >= std::min(a.x(), b.x())) && (p.x() <= std::max(a.x(), b.x()));
    bool y_ok = (p.y() >= std::min(a.y(), b.y())) && (p.y() <= std::max(a.y(), b.y()));
    return x_ok && y_ok;
}

std::vector<std::pair<QPointF, double>> Polygon::getRayPoints(const QPointF& lightSource) const
{
    std::vector<std::pair<QPointF, double>> points;

    for (size_t i = 0; i < m_vertices.size(); ++i) {
        const QPointF& vertex = m_vertices[i];
        const QPointF& prev = m_vertices[(i + m_vertices.size() - 1) % m_vertices.size()];
        const QPointF& next = m_vertices[(i + 1) % m_vertices.size()];

        // Основная вершина
        double angle = getAngle(lightSource, vertex);
        points.emplace_back(vertex, angle);

        // Левый касательный луч
        QPointF left_dir(prev.y() - vertex.y(), vertex.x() - prev.x());
        QPointF left_ray_end(vertex.x() + left_dir.x(), vertex.y() + left_dir.y());
        Ray left_ray(lightSource, left_ray_end);
        auto left_intersect = intersectRay(left_ray);
        if (left_intersect) {
            double left_angle = getAngle(lightSource, *left_intersect);
            points.emplace_back(*left_intersect, left_angle - 0.00001);
        }

        // Правый касательный луч
        QPointF right_dir(vertex.y() - next.y(), next.x() - vertex.x());
        QPointF right_ray_end(vertex.x() + right_dir.x(), vertex.y() + right_dir.y());
        Ray right_ray(lightSource, right_ray_end);
        auto right_intersect = intersectRay(right_ray);
        if (right_intersect) {
            double right_angle = getAngle(lightSource, *right_intersect);
            points.emplace_back(*right_intersect, right_angle + 0.00001);
        }
    }

    return points;
}

double Polygon::getAngle(const QPointF& center, const QPointF& point) const
{
    return atan2(point.y() - center.y(), point.x() - center.x());
}

size_t Polygon::vertexCount() const
{
    return m_vertices.size();
}

bool Polygon::empty() const
{
    return m_vertices.empty();
}

bool Polygon::isVertex(const QPointF& point) const
{
    const double eps = 1e-8;
    for (const auto& vertex : m_vertices) {
        double dx = point.x() - vertex.x();
        double dy = point.y() - vertex.y();
        if (dx*dx + dy*dy < eps) {
            return true;
        }
    }
    return false;
}
