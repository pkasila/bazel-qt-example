#include "../input/polygon.h"
#include "../input/ray.h"
#include <limits>
#include <cmath>
#include <vector>

Polygon::Polygon(std::vector<QPointF> vertices) : vertices_(std::move(vertices)) {}

void Polygon::addVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::updateLastVertex(const QPointF& new_vertex) {
    if (!vertices_.empty()) {
        vertices_.back() = new_vertex;
    }
}

std::optional<QPointF> Polygon::intersectSegments(const QPointF& p1, const QPointF& p2,
                                                  const QPointF& p3, const QPointF& p4) const
{
    const double x1 = p1.x();
    const double y1 = p1.y();
    const double x2 = p2.x();
    const double y2 = p2.y();
    const double x3 = p3.x();
    const double y3 = p3.y();
    const double x4 = p4.x();
    const double y4 = p4.y();
    const double den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    constexpr double epsilon = 1e-9;

    if (std::abs(den) < epsilon) {
        return std::nullopt;
    }

    const double t_num = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
    const double u_num = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3));

    const double t = t_num / den;
    const double u = u_num / den;


    if (t >= -epsilon && t <= 1.0 + epsilon && u >= -epsilon && u <= 1.0 + epsilon) {
        QPointF intersection_point;
        intersection_point.setX(x1 + t * (x2 - x1));
        intersection_point.setY(y1 + t * (y2 - y1));
        return intersection_point;
    }
    return std::nullopt;
}

std::optional<QPointF> Polygon::intersectRay(const Ray& ray) const {
    if (vertices_.size() < 2) {
        return std::nullopt;
    }
    const QPointF ray_start = ray.getBegin();
    const QPointF ray_end = ray.getEnd();
    std::optional<QPointF> closest_intersection = std::nullopt;
    double min_dist_sq = std::numeric_limits<double>::max();
    constexpr double origin_epsilon_sq = 1e-9;

    for (size_t i = 0; i < vertices_.size(); ++i) {
        const QPointF& edge_start = vertices_[i];
        const QPointF& edge_end = vertices_[(i + 1) % vertices_.size()];
        const std::optional<QPointF> current_intersection = intersectSegments(ray_start, ray_end, edge_start, edge_end);
        if (current_intersection) {
            const double current_dist_sq = distSq(ray_start, *current_intersection);
            if (current_dist_sq < origin_epsilon_sq) {
                continue;
            }
            if (current_dist_sq < min_dist_sq) {
                 min_dist_sq = current_dist_sq;
                 closest_intersection = current_intersection;
            }
        }
    }
    return closest_intersection;
}

bool Polygon::containsPoint(const QPointF& point) const {
    if (vertices_.size() < 3) {
         return false;
    }

    int crossings = 0;
    const size_t n = vertices_.size();
    for (size_t i = 0; i < n; ++i) {
        const QPointF& p1 = vertices_[i];
        const QPointF& p2 = vertices_[(i + 1) % n];
        if (((p1.y() <= point.y() && point.y() < p2.y()) ||
             (p2.y() <= point.y() && point.y() < p1.y())) &&
            (point.x() < (p2.x() - p1.x()) * (point.y() - p1.y()) / (p2.y() - p1.y()) + p1.x())) {
            crossings++;
        }
    }
    return (crossings % 2 == 1);
}

bool Polygon::intersects(const Polygon& other) const {
    const size_t n = this->vertexCount();
    const size_t m = other.vertexCount();
    if (n < 3 || m < 3) {
         return false;
    }

    for (size_t i = 0; i < n; ++i) {
        const QPointF& p1 = vertices_[i];
        const QPointF& p2 = vertices_[(i + 1) % n];
        for (size_t j = 0; j < m; ++j) {
            const QPointF& p3 = other.vertices_[j];
            const QPointF& p4 = other.vertices_[(j + 1) % m];
            if (intersectSegments(p1, p2, p3, p4).has_value()) {
                return true;
            }
        }
    }

    if (other.containsPoint(this->vertices_[0])) {
        return true;
    }
    if (this->containsPoint(other.vertices_[0])) {
        return true;
    }

    return false;
}