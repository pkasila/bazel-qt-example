#include "Polygon.h"

#include "geometry/GeometryUtils.h"

#include <cmath>
#include <limits>

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {
}

const std::vector<QPointF>& Polygon::GetVertices() const {
    return vertices_;
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (vertices_.empty()) {
        return;
    }
    vertices_.back() = new_vertex;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (vertices_.empty() || vertices_.size() < 2) {
        return std::nullopt;
    }

    double min_d = std::numeric_limits<double>::max();
    std::optional<QPointF> min_intersection;

    for (int i = 1; i <= vertices_.size(); ++i) {
        std::optional<QPointF> intersection;
        if (i == vertices_.size()) {
            intersection = geometry::IntersectRaySegment(
                ray.GetBegin(), ray.GetEnd() - ray.GetBegin(), vertices_[i - 1], vertices_[0]);
        } else {
            intersection = geometry::IntersectRaySegment(
                ray.GetBegin(), ray.GetEnd() - ray.GetBegin(), vertices_[i - 1], vertices_[i]);
        }

        if (intersection != std::nullopt) {
            double dist = geometry::DistanceSquared(*intersection, ray.GetBegin());
            if (dist < min_d) {
                min_d = dist;
                min_intersection = intersection;
            }
        }
    }

    if (min_d != std::numeric_limits<double>::max()) {
        return min_intersection;
    }

    return std::nullopt;
}

void Polygon::RemoveLastVertex() {
    if (!vertices_.empty()) {
        vertices_.pop_back();
    }
}

size_t Polygon::VertexCount() const {
    return vertices_.size();
}

bool Polygon::Empty() const {
    return vertices_.empty();
}

bool Polygon::ContainsPoint(const QPointF& point) const {
    if (vertices_.size() < 3) {
        return false;
    }

    bool inside = false;
    for (size_t i = 0, j = vertices_.size() - 1; i < vertices_.size(); j = i++) {
        const QPointF& a = vertices_[i];
        const QPointF& b = vertices_[j];

        const bool intersects =
            ((a.y() > point.y()) != (b.y() > point.y())) &&
            (point.x() < (b.x() - a.x()) * (point.y() - a.y()) / (b.y() - a.y()) + a.x());

        if (intersects) {
            inside = !inside;
        }
    }

    return inside;
}

bool Polygon::IntersectsPolygon(const Polygon& other, bool this_closed) const {
    const auto& a = vertices_;
    const auto& b = other.GetVertices();

    if (a.size() < 2 || b.size() < 2) {
        return false;
    }

    const size_t a_edges = this_closed ? a.size() : a.size() - 1;
    const size_t b_edges = b.size();

    for (size_t i = 0; i < a_edges; ++i) {
        const QPointF a1 = a[i];
        const QPointF a2 = this_closed ? a[(i + 1) % a.size()] : a[i + 1];

        for (size_t j = 0; j < b_edges; ++j) {
            const QPointF b1 = b[j];
            const QPointF b2 = b[(j + 1) % b.size()];

            if (geometry::SegmentsIntersect(a1, a2, b1, b2)) {
                return true;
            }
        }
    }

    if (this_closed && !a.empty() && other.ContainsPoint(a[0])) {
        return true;
    }

    if (!b.empty() && ContainsPoint(b[0])) {
        return true;
    }

    return false;
}

bool Polygon::SelfIntersects(bool closed) const {
    const size_t n = vertices_.size();

    if (closed) {
        if (n < 4) {
            return false;
        }

        for (size_t i = 0; i < n; ++i) {
            const QPointF a1 = vertices_[i];
            const QPointF a2 = vertices_[(i + 1) % n];

            for (size_t j = i + 1; j < n; ++j) {
                const size_t i_next = (i + 1) % n;
                const size_t j_next = (j + 1) % n;

                if (i == j || i == j_next || i_next == j) {
                    continue;
                }

                const QPointF b1 = vertices_[j];
                const QPointF b2 = vertices_[j_next];

                if (geometry::SegmentsIntersect(a1, a2, b1, b2)) {
                    return true;
                }
            }
        }

        return false;
    }

    if (n < 4) {
        return false;
    }

    for (size_t i = 0; i + 1 < n; ++i) {
        const QPointF a1 = vertices_[i];
        const QPointF a2 = vertices_[i + 1];

        for (size_t j = i + 1; j + 1 < n; ++j) {
            if (j == i || j == i + 1) {
                continue;
            }

            const QPointF b1 = vertices_[j];
            const QPointF b2 = vertices_[j + 1];

            if (geometry::SegmentsIntersect(a1, a2, b1, b2)) {
                return true;
            }
        }
    }

    return false;
}

bool Polygon::OnBoundary(const QPointF& point) const {
    if (vertices_.size() < 2) {
        return false;
    }

    for (size_t i = 0; i < vertices_.size(); ++i) {
        const QPointF a = vertices_[i];
        const QPointF b = vertices_[(i + 1) % vertices_.size()];

        if (geometry::OnSegment(a, b, point)) {
            return true;
        }
    }

    return false;
}
