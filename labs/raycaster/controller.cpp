#include "controller.h"

#include <algorithm>
#include <cmath>

namespace {

constexpr double kEps = 0.0001;
constexpr double kRayLength = 5000.0;

}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF& vertex) {

    if (!polygons_.empty()) {
        polygons_.back().AddVertex(vertex);
    }
}

void Controller::UpdateLastPolygon(const QPointF& vertex) {

    if (!polygons_.empty()) {
        polygons_.back().UpdateLastVertex(vertex);
    }
}

void Controller::FinishLastPolygon() {

    if (polygons_.empty()) {
        return;
    }

    auto& vertices = polygons_.back().GetVertices();

    if (vertices.size() >= 2) {
        vertices.pop_back();
    }
}

void Controller::SetLightSource(const QPointF& source) {
    light_source_ = source;
}

const QPointF& Controller::GetLightSource() const {
    return light_source_;
}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

std::vector<Ray> Controller::CastRays() const {

    std::vector<Ray> rays;

    for (const auto& polygon : polygons_) {

        for (const auto& vertex : polygon.GetVertices()) {

            double angle = std::atan2(
                vertex.y() - light_source_.y(),
                vertex.x() - light_source_.x()
                );

            QPointF end(
                light_source_.x() + std::cos(angle) * kRayLength,
                light_source_.y() + std::sin(angle) * kRayLength
                );

            Ray ray(light_source_, end, angle);

            rays.push_back(ray.Rotate(-kEps));
            rays.push_back(ray);
            rays.push_back(ray.Rotate(kEps));
        }
    }

    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {

    for (auto& ray : *rays) {

        QPointF closest = ray.GetEnd();

        double min_dist = kRayLength;

        for (const auto& polygon : polygons_) {

            QPointF point = polygon.IntersectRay(ray);

            double dx = point.x() - light_source_.x();
            double dy = point.y() - light_source_.y();

            double dist = std::sqrt(dx * dx + dy * dy);

            if (dist < min_dist) {
                min_dist = dist;
                closest = point;
            }
        }

        ray.SetEnd(closest);
    }
}

Polygon Controller::CreateLightArea() const {

    std::vector<Ray> rays = CastRays();

    IntersectRays(&rays);

    std::sort(
        rays.begin(),
        rays.end(),
        [](const Ray& a, const Ray& b) {
            return a.GetAngle() < b.GetAngle();
        }
        );

    Polygon polygon;

    for (const auto& ray : rays) {
        polygon.AddVertex(ray.GetEnd());
    }

    return polygon;
}
