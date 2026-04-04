#include "polygon.h"

#include <cmath>
#include <limits>

namespace {
constexpr double kEps = 1e-9;

double Cross(const QPointF& a, const QPointF& b) {
    return a.x() * b.y() - a.y() * b.x();
}

double Dot(const QPointF& a, const QPointF& b) {
    return a.x() * b.x() + a.y() * b.y();
}

double LengthSquared(const QPointF& v) {
    return Dot(v, v);
}

bool NearlyEqual(double a, double b) {
    return std::abs(a - b) <= kEps;
}
}

Polygon::Polygon() = default;

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {}

const std::vector<QPointF>& Polygon::GetVertices() const {
    return vertices_;
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (!vertices_.empty()) {
        vertices_.back() = new_vertex;
    }
}

void Polygon::RemoveLastVertex() {
    if (!vertices_.empty()) {
        vertices_.pop_back();
    }
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (vertices_.size() < 2) {
        return std::nullopt;
    }

    const QPointF p = ray.GetBegin();
    const QPointF r = ray.GetEnd() - ray.GetBegin();
    const double ray_length_squared = LengthSquared(r);
    if (ray_length_squared <= kEps) {
        return std::nullopt;
    }

    bool found = false;
    double best_t = std::numeric_limits<double>::infinity();
    QPointF best_point;

    for (std::size_t i = 0; i < vertices_.size(); ++i) {
        const QPointF q = vertices_[i];
        const QPointF next = vertices_[(i + 1) % vertices_.size()];
        const QPointF s = next - q;
        const double denominator = Cross(r, s);
        const QPointF qp = q - p;

        if (NearlyEqual(denominator, 0.0)) {
            if (!NearlyEqual(Cross(qp, r), 0.0)) {
                continue;
            }

            const double t0 = Dot(q - p, r) / ray_length_squared;
            const double t1 = Dot(next - p, r) / ray_length_squared;
            double candidate_t = std::numeric_limits<double>::infinity();

            if (t0 >= 0.0) {
                candidate_t = std::min(candidate_t, t0);
            }
            if (t1 >= 0.0) {
                candidate_t = std::min(candidate_t, t1);
            }
            if (!std::isfinite(candidate_t)) {
                continue;
            }
            if (candidate_t < best_t) {
                best_t = candidate_t;
                best_point = p + r * candidate_t;
                found = true;
            }
            continue;
        }

        const double t = Cross(qp, s) / denominator;
        const double u = Cross(qp, r) / denominator;

        if (t >= 0.0 && u >= -kEps && u <= 1.0 + kEps) {
            if (t < best_t) {
                best_t = t;
                best_point = p + r * t;
                found = true;
            }
        }
    }

    if (!found) {
        return std::nullopt;
    }

    return best_point;
}
