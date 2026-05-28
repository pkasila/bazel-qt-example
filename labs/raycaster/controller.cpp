#include "controller.h"

#include <algorithm>
#include <cmath>

namespace {

double DistanceSquared(const QPointF& a, const QPointF& b) {
    double dx = a.x() - b.x();
    double dy = a.y() - b.y();
    return dx * dx + dy * dy;
}

}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().AddVertex(new_vertex);
    }
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().UpdateLastVertex(new_vertex);
    }
}

QPointF Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF& point) {
    light_source_ = point;
}

std::vector<Ray> Controller::CastRays() const {
    std::vector<Ray> rays;

    const double ray_length = 5000.0;

    for (const auto& polygon : polygons_) {
        for (const auto& vertex : polygon.GetVertices()) {

            double angle = std::atan2(
                vertex.y() - light_source_.y(),
                vertex.x() - light_source_.x()
                );

            QPointF end(
                light_source_.x() + std::cos(angle) * ray_length,
                light_source_.y() + std::sin(angle) * ray_length
                );

            Ray base(light_source_, end, angle);

            rays.push_back(base);
            rays.push_back(base.Rotate(0.0001));
            rays.push_back(base.Rotate(-0.0001));
        }
    }

    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    for (auto& ray : *rays) {
        for (const auto& polygon : polygons_) {
            auto intersection = polygon.IntersectRay(ray);

            if (intersection.has_value()) {
                if (DistanceSquared(ray.GetBegin(), intersection.value()) <
                    DistanceSquared(ray.GetBegin(), ray.GetEnd())) {

                    ray.SetEnd(intersection.value());
                }
            }
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    const double eps = 1.0;

    std::vector<Ray> filtered;

    for (const auto& ray : *rays) {
        bool close = false;

        for (const auto& other : filtered) {
            if (DistanceSquared(ray.GetEnd(), other.GetEnd()) < eps * eps) {
                close = true;
                break;
            }
        }

        if (!close) {
            filtered.push_back(ray);
        }
    }

    *rays = filtered;
}

Polygon Controller::CreateLightArea() const {
    auto rays = CastRays();

    IntersectRays(&rays);

    std::sort(rays.begin(), rays.end(),
              [](const Ray& a, const Ray& b) {
                  return a.GetAngle() < b.GetAngle();
              });

    std::vector<QPointF> vertices;

    for (const auto& ray : rays) {
        vertices.push_back(ray.GetEnd());
    }

    return Polygon(vertices);
}
