#include "Controller.h"

#include "geometry/GeometryUtils.h"

#include <algorithm>
#include <cmath>

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (polygons_.empty()) {
        return;
    }
    polygons_.back().AddVertex(new_vertex);
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (polygons_.empty()) {
        return;
    }
    polygons_.back().UpdateLastVertex(new_vertex);
}

const QPointF& Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF& light_source) {
    light_source_ = light_source;
}

std::vector<Ray> Controller::CastRays() const {
    const double del_angle = 0.0001;
    const double ray_length = 10000.0;

    std::vector<Ray> res;

    for (const auto& polygon : polygons_) {
        for (const auto& vertex : polygon.GetVertices()) {
            QPointF dir = vertex - light_source_;

            double len2 = geometry::LengthSquared(dir);
            if (len2 < 1e-12) {
                continue;
            }

            double len = std::sqrt(len2);
            QPointF normalized = dir / len;
            QPointF far_end = light_source_ + normalized * ray_length;

            double angle = geometry::AngleOf(dir);
            Ray ray(light_source_, far_end, angle);

            res.push_back(ray);
            res.push_back(ray.Rotate(del_angle));
            res.push_back(ray.Rotate(-del_angle));
        }
    }

    return res;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    for (auto& ray : *rays) {
        for (const auto& polygon : polygons_) {
            std::optional<QPointF> intersect = polygon.IntersectRay(ray);
            if (intersect == std::nullopt) {
                continue;
            }

            if (geometry::DistanceSquared(ray.GetBegin(), *intersect) <
                geometry::DistanceSquared(ray.GetBegin(), ray.GetEnd())) {
                ray.SetEnd(*intersect);
            }
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    if (rays == nullptr || rays->size() <= 1) {
        return;
    }

    const double eps = 1e-8;
    std::vector<Ray> filtered;
    filtered.push_back((*rays)[0]);

    for (size_t i = 1; i < rays->size(); ++i) {
        const QPointF& current_end = (*rays)[i].GetEnd();
        const QPointF& last_end = filtered.back().GetEnd();

        if (geometry::DistanceSquared(current_end, last_end) > eps) {
            filtered.push_back((*rays)[i]);
        }
    }

    if (filtered.size() > 1) {
        const QPointF& first_end = filtered.front().GetEnd();
        const QPointF& last_end = filtered.back().GetEnd();

        if (geometry::DistanceSquared(first_end, last_end) <= eps) {
            filtered.pop_back();
        }
    }

    *rays = std::move(filtered);
}

Polygon Controller::CreateLightArea() const {
    std::vector<Ray> rays = CastRays();

    IntersectRays(&rays);

    std::ranges::sort(rays, [](const Ray& a, const Ray& b) {
        return a.GetAngle() < b.GetAngle();
    });

    RemoveAdjacentRays(&rays);

    std::vector<QPointF> vertices;
    for (const auto& ray : rays) {
        vertices.push_back(ray.GetEnd());
    }

    return Polygon(vertices);
}