#include "polygon.h"
#include "ray.h"
#include <algorithm>
#include <cmath>
#include <limits>

Polygon::Polygon(const std::vector<QPoint>& vertices) : vertices_(vertices) {
}

void Polygon::AddVertex(const QPoint& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPoint& new_vertex) {
    if (!vertices_.empty()) {
        vertices_.back() = new_vertex;
    }
}

std::optional<QPoint> Polygon::IntersectRay(const Ray& ray) const {
    if (vertices_.size() < 2) {
        return std::nullopt;
    }
    
    QPoint ray_start = ray.getBegin();
    QPoint ray_end = ray.getEnd();
    
    std::optional<QPoint> closest_intersection;
    double min_t1 = std::numeric_limits<double>::infinity();

    for (size_t i = 0; i < vertices_.size(); ++i) {
        size_t next_i = (i + 1) % vertices_.size();
        QPoint seg_start = vertices_[i];
        QPoint seg_end = vertices_[next_i];
        
        auto intersection_result = IntersectRaySegment(ray_start, ray_end, seg_start, seg_end);
        if (intersection_result) {
            auto [intersection_point, t1] = *intersection_result;
            if (t1 < min_t1) {
                min_t1 = t1;
                closest_intersection = intersection_point;
            }
        }
    }
    
    return closest_intersection;
}

std::optional<std::pair<QPoint, double>> Polygon::IntersectRaySegment(const QPoint& ray_start, const QPoint& ray_end,
                                                                      const QPoint& seg_start, const QPoint& seg_end) const {
    double r_px = ray_start.x();
    double r_py = ray_start.y();
    double r_dx = ray_end.x() - ray_start.x();
    double r_dy = ray_end.y() - ray_start.y();

    double s_px = seg_start.x();
    double s_py = seg_start.y();
    double s_dx = seg_end.x() - seg_start.x();
    double s_dy = seg_end.y() - seg_start.y();

    double denom = s_dx * r_dy - s_dy * r_dx;
    if (std::abs(denom) < 1e-10) {
        return std::nullopt;
    }

    double t2 = (r_dx * (s_py - r_py) + r_dy * (r_px - s_px)) / denom;

    double t1;
    if (std::abs(r_dx) > std::abs(r_dy)) {
        t1 = (s_px + s_dx * t2 - r_px) / r_dx;
    } else {
        t1 = (s_py + s_dy * t2 - r_py) / r_dy;
    }

    if (t1 > 0 && t2 > 0 && t2 < 1) {
        if (t1 < 1e-6) {
            return std::nullopt;
        }

        double intersection_x = r_px + r_dx * t1;
        double intersection_y = r_py + r_dy * t1;
        
        return std::make_pair(QPoint(static_cast<int>(intersection_x), static_cast<int>(intersection_y)), t1);
    }
    
    return std::nullopt;
}
