#include "controller.h"

#include <algorithm>
#include <cmath>
#include <limits>

Controller::Controller(const std::vector<Polygon>& polygons, const std::vector<Ray>& rays)
    : polygons_(polygons), rays_(rays) {
    light_systems_.emplace_back();
    light_systems_[0].light_sources.resize(5);
}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddVertexToLastPolygon(const QPoint& new_vertex) {
    polygons_.back().AddVertex(new_vertex);
}

void Controller::UpdateLastPolygon(const QPoint& new_vertex) {
    polygons_.back().UpdateLastVertex(new_vertex);
}

const std::vector<QPoint>& Controller::GetDynamicLightSource() const {
    return light_systems_[0].light_sources;
}

std::vector<QPoint> Controller::GetLightSources() const {
    std::vector<QPoint> result;
    result.reserve(5 * light_systems_.size());
    for (const auto& light_system : light_systems_) {
        result.insert(
            result.end(), light_system.light_sources.begin(), light_system.light_sources.end());
    }
    return result;
}

const std::vector<LightSystem>& Controller::GetLightSystems() const {
    return light_systems_;
}

void Controller::SetLightSource(const QPoint& center) {
    light_systems_.clear();
    light_systems_.emplace_back();
    light_systems_[0].light_sources.resize(5);
    SetDynamicLightSource(center);
}

void Controller::SetDynamicLightSource(const QPoint& center) {
    if (light_systems_.empty()) {
        return;
    }
    if (light_systems_[0].light_sources.size() < 5) {
        light_systems_[0].light_sources.resize(5);
    }
    light_systems_[0].light_sources[0] = center;
    light_systems_[0].light_sources[1] = center + QPoint(5, 5);
    light_systems_[0].light_sources[2] = center + QPoint(-5, 5);
    light_systems_[0].light_sources[3] = center + QPoint(5, -5);
    light_systems_[0].light_sources[4] = center + QPoint(-5, -5);
}

void Controller::AddStaticLightSource(const QPoint& center) {
    LightSystem new_light_system;
    new_light_system.light_sources.resize(5);
    new_light_system.light_sources[0] = center;
    new_light_system.light_sources[1] = center + QPoint(5, 5);
    new_light_system.light_sources[2] = center + QPoint(-5, 5);
    new_light_system.light_sources[3] = center + QPoint(5, -5);
    new_light_system.light_sources[4] = center + QPoint(-5, -5);
    light_systems_.push_back(new_light_system);
}

void Controller::RemoveLastLightSource() {
    if (!light_systems_.empty()) {
        light_systems_.pop_back();
    }
}

std::vector<Ray> Controller::CastRays(const QPoint& source) {
    std::vector<Ray> casted_rays;
    for (const auto& poly : polygons_) {
        for (const auto& vertex : poly.GetAllVertices()) {
            double angle = std::atan2(vertex.y() - source.y(), vertex.x() - source.x());
            casted_rays.emplace_back(source, vertex, angle);
            casted_rays.emplace_back(source, QPoint(), angle + 0.0001);
            casted_rays.emplace_back(source, QPoint(), angle - 0.0001);
        }
    }
    return casted_rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays, const QPoint& source) {
    if (!rays) {
        return;
    }
    for (auto& ray : *rays) {
        std::optional<QPoint> closest;
        double min_dist_sq = std::numeric_limits<double>::max();

        for (const auto& poly : polygons_) {
            auto hit = poly.IntersectRay(ray);
            if (hit) {
                double dx = hit->x() - source.x();
                double dy = hit->y() - source.y();
                double dist_sq = (dx * dx) + (dy * dy);
                if (dist_sq < min_dist_sq) {
                    min_dist_sq = dist_sq;
                    closest = hit;
                }
            }
        }
        if (closest) {
            ray.SetEnd(*closest);
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) {
    if (!rays || rays->size() < 2) {
        return;
    }
    auto it = std::unique(rays->begin(), rays->end(), [](const Ray& a, const Ray& b) {
        const QPoint p1 = a.GetEnd();
        const QPoint p2 = b.GetEnd();
        double dx = p1.x() - p2.x();
        double dy = p1.y() - p2.y();
        return ((dx * dx) + (dy * dy)) < 1.0;
    });
    rays->erase(it, rays->end());
}

Polygon Controller::CreateLightArea(const QPoint& source) {
    std::vector<Ray> rays = CastRays(source);
    IntersectRays(&rays, source);

    std::sort(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) {
        return a.GetAngle() < b.GetAngle();
    });

    RemoveAdjacentRays(&rays);

    std::vector<QPoint> vertices;
    for (const auto& ray : rays) {
        vertices.push_back(ray.GetEnd());
    }
    return Polygon(vertices);
}

void Controller::ReplacePolygon(size_t index, const Polygon& polygon) {
    if (index < polygons_.size()) {
        polygons_[index] = polygon;
    }
}
