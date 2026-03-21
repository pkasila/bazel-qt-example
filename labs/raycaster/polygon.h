#pragma once
#include "ray.h"

#include <QPointF>
#include <algorithm>
#include <limits>
#include <optional>
#include <vector>

class Polygon {
   public:
    Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {
    }

    const std::vector<QPointF>& GetVertices() const {
        return vertices_;
    }

    void AddVertex(const QPointF& v) {
        vertices_.push_back(v);
    }

    void UpdateLastVertex(const QPointF& v) {
        if (!vertices_.empty()) {
            vertices_.back() = v;
        }
    }

    std::optional<QPointF> IntersectRay(const Ray& ray) const {
        std::optional<QPointF> closest;
        double min_t = std::numeric_limits<double>::max();

        const QPointF p1 = ray.getBegin();
        const double dx = std::cos(ray.getAngle());
        const double dy = std::sin(ray.getAngle());

        for (size_t i = 0; i < vertices_.size(); ++i) {
            QPointF p3 = vertices_[i];
            QPointF p4 = vertices_[(i + 1) % vertices_.size()];

            double sx = p4.x() - p3.x();
            double sy = p4.y() - p3.y();

            double den = dx * sy - dy * sx;
            if (std::abs(den) < 1e-9) {
                continue;
            }

            double t = ((p3.x() - p1.x()) * sy - (p3.y() - p1.y()) * sx) / den;
            double u = ((p3.x() - p1.x()) * dy - (p3.y() - p1.y()) * dx) / den;

            if (t > 1e-4 && u >= 0 && u <= 1) {
                if (t < min_t) {
                    min_t = t;
                    closest = QPointF(p1.x() + t * dx, p1.y() + t * dy);
                }
            }
        }
        return closest;
    }

    bool IsPointNearBoundary(const QPointF& p, double threshold) const {
        for (size_t i = 0; i < vertices_.size(); ++i) {
            QPointF a = vertices_[i];
            QPointF b = vertices_[(i + 1) % vertices_.size()];

            double l2 = std::pow(b.x() - a.x(), 2) + std::pow(b.y() - a.y(), 2);
            if (l2 == 0) {
                continue;
            }

            double t = std::max(
                0.0,
                std::min(
                    1.0,
                    ((p.x() - a.x()) * (b.x() - a.x()) + (p.y() - a.y()) * (b.y() - a.y())) / l2));
            QPointF proj(a.x() + t * (b.x() - a.x()), a.y() + t * (b.y() - a.y()));

            double distSq = std::pow(p.x() - proj.x(), 2) + std::pow(p.y() - proj.y(), 2);
            if (distSq < threshold * threshold) {
                return true;
            }
        }
        return false;
    }

   private:
    std::vector<QPointF> vertices_;
};