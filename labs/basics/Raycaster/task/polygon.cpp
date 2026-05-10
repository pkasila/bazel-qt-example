#include "polygon.h"

#include <cmath>
#include <limits>

QPoint Polygon::GetVertice(size_t index) const {
    return v_[index];
}

const std::vector<QPoint>& Polygon::GetAllVertices() const {
    return v_;
}

void Polygon::SetVertice(QPoint point, size_t index) {
    if (index == static_cast<size_t>(-1)) {
        v_.push_back(point);
        return;
    }
    v_[index] = point;
}

void Polygon::SetAllVertices(const std::vector<QPoint>& vertices) {
    v_ = vertices;
}

void Polygon::AddVertex(const QPoint& vertex) {
    SetVertice(vertex);
}

void Polygon::UpdateLastVertex(const QPoint& new_vertex) {
    SetVertice(new_vertex, v_.size() - 1);
}

std::optional<QPoint> Polygon::IntersectRay(const Ray& ray) const {
    if (v_.size() < 2) {
        return std::nullopt;
    }

    double min_t = std::numeric_limits<double>::max();
    std::optional<QPoint> closest_point;

    const QPoint p = ray.GetBegin();
    double cos_a = std::cos(ray.GetAngle());
    double sin_a = std::sin(ray.GetAngle());

    for (size_t i = 0; i < v_.size(); ++i) {
        const QPoint a = v_[i];
        const QPoint b = v_[(i + 1) % v_.size()];
        const QPoint v = b - a;

        double det = (v.x() * sin_a) - (v.y() * cos_a);
        if (std::abs(det) < 1e-9) {
            continue;
        }

        double t = (((a.y() - p.y()) * v.x()) - ((a.x() - p.x()) * v.y())) / det;
        double u = ((cos_a * (a.y() - p.y())) - (sin_a * (a.x() - p.x()))) / det;

        if (t >= 0 && u >= 0 && u <= 1) {
            if (t < min_t) {
                min_t = t;
                closest_point = QPoint(
                    static_cast<int>(std::round(p.x() + (t * cos_a))),
                    static_cast<int>(std::round(p.y() + (t * sin_a))));
            }
        }
    }

    return closest_point;
}

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Segment_2.h>
#include <CGAL/squared_distance_2.h>

using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
using Point = CGAL::Point_2<Kernel>;
using Segment = CGAL::Segment_2<Kernel>;

QPoint Polygon::GetDistanceVector(const QPoint& point) const {
    if (v_.empty()) {
        return {0, 0};
    }

    Point p(point.x(), point.y());
    Kernel::FT min_dist_sq;
    Point closest_p;
    bool first = true;

    for (size_t i = 0; i < v_.size(); ++i) {
        Segment s(
            Point(v_[i].x(), v_[i].y()),
            Point(v_[(i + 1) % v_.size()].x(), v_[(i + 1) % v_.size()].y()));

        auto line = s.supporting_line();
        auto proj = line.projection(p);
        Point current_closest;
        if (s.has_on(proj)) {
            current_closest = proj;
        } else {
            auto d1 = CGAL::squared_distance(p, s.source());
            auto d2 = CGAL::squared_distance(p, s.target());
            current_closest = (d1 < d2) ? s.source() : s.target();
        }

        auto dist_sq = CGAL::squared_distance(p, current_closest);
        if (first || dist_sq < min_dist_sq) {
            min_dist_sq = dist_sq;
            closest_p = current_closest;
            first = false;
        }
    }

    return {
      static_cast<int>(std::round(CGAL::to_double(closest_p.x()) - point.x())),
      static_cast<int>(std::round(CGAL::to_double(closest_p.y()) - point.y()))};
}
