#include "controller.h"

#include <algorithm>
#include <cmath>

Controller::Controller() : light_source_(QPointF(500, 500)) {
}

void Controller::UpdateBounds(double width, double height) {
    std::vector<QPointF> bounds = {
      QPointF(0, 0), QPointF(width, 0), QPointF(width, height), QPointF(0, height)};
    if (polygons_.empty()) {
        polygons_.push_back(Polygon(bounds));
    } else {
        polygons_[0] = Polygon(bounds);
    }
}

void Controller::AddVertexToLastPolygon(const QPointF& v) {
    if (polygons_.size() > 1) {
        polygons_.back().AddVertex(v);
    }
}

void Controller::UpdateLastPolygon(const QPointF& v) {
    if (polygons_.size() > 1) {
        polygons_.back().UpdateLastVertex(v);
    }
}

bool Controller::CheckCollision(const QPointF& p, double threshold) const {
    for (size_t i = 1; i < polygons_.size(); ++i) {
        if (polygons_[i].IsPointNearBoundary(p, threshold)) {
            return true;
        }
    }
    return false;
}

std::vector<Ray> Controller::CastRays() {
    std::vector<QPointF> targets;
    std::vector<std::pair<QPointF, QPointF>> edges;

    // 1. Собираем все вершины и все ребра
    for (const auto& poly : polygons_) {
        const auto& v = poly.GetVertices();
        if (v.empty()) {
            continue;
        }
        for (size_t i = 0; i < v.size(); ++i) {
            targets.push_back(v[i]);
            edges.push_back({v[i], v[(i + 1) % v.size()]});
        }
    }

    // 2. Находим пересечения всех ребер (чтобы тени не протекали сквозь пересекающиеся полигоны)
    for (size_t i = 0; i < edges.size(); ++i) {
        for (size_t j = i + 1; j < edges.size(); ++j) {
            QPointF a = edges[i].first, b = edges[i].second;
            QPointF c = edges[j].first, d = edges[j].second;

            double den = (b.x() - a.x()) * (d.y() - c.y()) - (b.y() - a.y()) * (d.x() - c.x());
            if (std::abs(den) < 1e-9) {
                continue;  // Параллельны
            }

            double t =
                ((c.x() - a.x()) * (d.y() - c.y()) - (c.y() - a.y()) * (d.x() - c.x())) / den;
            double u =
                ((c.x() - a.x()) * (b.y() - a.y()) - (c.y() - a.y()) * (b.x() - a.x())) / den;

            // Если есть точка пересечения, добавляем ее как цель для луча
            if (t > 1e-4 && t < 1.0 - 1e-4 && u > 1e-4 && u < 1.0 - 1e-4) {
                targets.push_back(
                    QPointF(a.x() + t * (b.x() - a.x()), a.y() + t * (b.y() - a.y())));
            }
        }
    }

    std::vector<Ray> rays;
    // 3. Пускаем лучи во все цели
    for (const auto& target : targets) {
        double angle = std::atan2(target.y() - light_source_.y(), target.x() - light_source_.x());

        QPointF end(
            light_source_.x() + std::cos(angle) * 3000, light_source_.y() + std::sin(angle) * 3000);
        Ray base_ray(light_source_, end, angle);

        rays.push_back(base_ray);
        rays.push_back(base_ray.Rotate(0.00001));
        rays.push_back(base_ray.Rotate(-0.00001));
    }

    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) {
    for (auto& ray : *rays) {
        std::optional<QPointF> best_hit;
        double min_d2 = std::pow(ray.getEnd().x() - ray.getBegin().x(), 2) +
                        std::pow(ray.getEnd().y() - ray.getBegin().y(), 2);

        for (const auto& poly : polygons_) {
            auto hit = poly.IntersectRay(ray);
            if (hit) {
                double d2 = std::pow(hit->x() - ray.getBegin().x(), 2) +
                            std::pow(hit->y() - ray.getBegin().y(), 2);
                if (d2 < min_d2) {
                    min_d2 = d2;
                    best_hit = hit;
                }
            }
        }
        if (best_hit) {
            ray.setEnd(*best_hit);
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) {
    if (rays->empty()) {
        return;
    }

    std::vector<Ray> filtered;
    filtered.push_back((*rays)[0]);

    for (size_t i = 1; i < rays->size(); ++i) {
        const QPointF& p1 = filtered.back().getEnd();
        const QPointF& p2 = (*rays)[i].getEnd();
        double dist2 = std::pow(p1.x() - p2.x(), 2) + std::pow(p1.y() - p2.y(), 2);

        if (dist2 > 1.0) {
            filtered.push_back((*rays)[i]);
        }
    }

    if (filtered.size() > 1) {
        const QPointF& p1 = filtered.back().getEnd();
        const QPointF& p2 = filtered.front().getEnd();
        double dist2 = std::pow(p1.x() - p2.x(), 2) + std::pow(p1.y() - p2.y(), 2);
        if (dist2 <= 1.0) {
            filtered.pop_back();
        }
    }

    *rays = std::move(filtered);
}

Polygon Controller::CreateLightArea() {
    auto rays = CastRays();
    IntersectRays(&rays);

    std::sort(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) {
        return a.getAngle() < b.getAngle();
    });

    RemoveAdjacentRays(&rays);

    std::vector<QPointF> vertices;
    for (const auto& r : rays) {
        vertices.push_back(r.getEnd());
    }

    return Polygon(vertices);
}