#include "controller.h"

#include <algorithm>

std::vector<Ray> Controller::CastRays(const QPointF& source) {
    std::vector<Ray> rays;
    for (const auto& poly : m_polygons) {
        for (const auto& v : poly.GetVertices()) {
            double angle = std::atan2(v.y() - source.y(), v.x() - source.x());
            Ray r(source, v, angle);
            rays.push_back(r);
            rays.push_back(r.Rotate(0.0001));
            rays.push_back(r.Rotate(-0.0001));
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) {
    for (auto& ray : *rays) {
        std::optional<QPointF> bestHit = std::nullopt;
        double minDistSq = 1e15;

        for (const auto& poly : m_polygons) {
            auto hit = poly.IntersectRay(ray);
            if (hit) {
                double dx = hit->x() - ray.getBegin().x();
                double dy = hit->y() - ray.getBegin().y();
                double dSq = dx * dx + dy * dy;
                if (dSq < minDistSq) {
                    minDistSq = dSq;
                    bestHit = hit;
                }
            }
        }
        if (bestHit) {
            ray.setEnd(*bestHit);
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) {
    if (rays->size() < 2) {
        return;
    }

    std::sort(rays->begin(), rays->end(), [](const Ray& a, const Ray& b) {
        return a.getAngle() < b.getAngle();
    });

    std::vector<Ray> filtered;
    filtered.reserve(rays->size());

    for (const auto& r : *rays) {
        if (filtered.empty()) {
            filtered.push_back(r);
            continue;
        }

        const QPointF& p1 = filtered.back().getEnd();
        const QPointF& p2 = r.getEnd();
        double dx = p1.x() - p2.x();
        double dy = p1.y() - p2.y();

        if ((dx * dx + dy * dy) > 1.0) {
            filtered.push_back(r);
        }
    }
    *rays = std::move(filtered);
}

Polygon Controller::CreateLightArea(const QPointF& source) {
    std::vector<Ray> rays = CastRays(source);
    IntersectRays(&rays);
    RemoveAdjacentRays(&rays);

    std::vector<QPointF> vertices;
    for (const auto& r : rays) {
        vertices.push_back(r.getEnd());
    }
    return Polygon(vertices);
}