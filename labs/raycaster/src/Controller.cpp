#include "Polygon.h"
#include "Controller.h"
#include <vector>
#include <QPoint>
#include <algorithm>

Controller::Controller() {
    polygons_.reserve(10);
}

Controller::Controller(const QColor& color) : color_(color) {
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

QColor Controller::GetColor() const {
    return color_;
}


void Controller::SetLightSource(const QPointF& point) {
    light_source_ = point;
}

std::vector<Ray> Controller::CastRays() {
    std::vector<Ray> rays;
    rays.reserve(200);

    for (const auto& polygon : polygons_) {
        for (const auto& vertex : polygon.GetVertices()) {
            double angle = std::atan2(vertex.y() - light_source_.y(), vertex.x() - light_source_.x());
            Ray ray(light_source_, vertex, angle);
            rays.push_back(ray.Rotate(0.0001));
            rays.push_back(ray.Rotate(-0.0001));
            rays.push_back(ray);
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) {
    for (auto& ray : *rays) {
        std::optional<QPointF> nearestPoint;
        double minDistSquared = std::pow(ray.GetEnd().x() - ray.GetBegin().x(), 2) +
                                std::pow(ray.GetEnd().y() - ray.GetBegin().y(), 2);

        for (const auto& polygon : polygons_) {
            std::optional<QPointF> intersection = polygon.IntersectRay(ray);
            if (intersection) {
                double distSquared = std::pow(intersection->x() - ray.GetBegin().x(), 2) +
                                     std::pow(intersection->y() - ray.GetBegin().y(), 2);
                if (distSquared < minDistSquared) {
                    minDistSquared = distSquared;
                    nearestPoint = intersection;
                }
            }
        }
        if (nearestPoint) {
            ray.SetEnd(*nearestPoint);
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) {
    const double epsilon = 5.0;
    if (rays->empty()) return;

    std::vector<Ray> temp;
    temp.reserve(rays->size());
    temp.push_back((*rays)[0]);

    for (size_t i = 1; i < rays->size(); ++i) {
        bool is_unique = true;
        for (const auto& unique_ray : temp) {
            double dx = (*rays)[i].GetEnd().x() - unique_ray.GetEnd().x();
            double dy = (*rays)[i].GetEnd().y() - unique_ray.GetEnd().y();
            double distSquared = dx * dx + dy * dy;
            if (distSquared < epsilon * epsilon) {
                is_unique = false;
                break;
            }
        }
        if (is_unique) {
            temp.push_back((*rays)[i]);
        }
    }

    *rays = std::move(temp);
}

Polygon Controller::CreateLightArea() {
    std::vector<Ray> rays = CastRays();
    IntersectRays(&rays);
    RemoveAdjacentRays(&rays);

    std::sort(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) {
        return a.GetAngle() < b.GetAngle();
    });

    std::vector<QPointF> light_area;
    light_area.reserve(rays.size());
    for (const auto& ray : rays) {
        light_area.push_back(ray.GetEnd());
    }
    return Polygon(light_area);
}
