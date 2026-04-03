//
// Created by blalex on 4/8/25.
//

#include "controller.h"

#include <algorithm>

Controller::Controller() {
    std::vector<QPointF> border = {
      QPointF(0, 0), QPointF(800, 0), QPointF(800, 600), QPointF(0, 600)};
    polygons_.emplace_back(border);
}

const std::vector<Polygon>& Controller::GetPolygons() {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& new_polygon) {
    polygons_.push_back(new_polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    polygons_.back().AddVertex(new_vertex);
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    polygons_.back().UpdateLastVertex(new_vertex);
}

QPointF Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF& light_source) {
    light_source_ = light_source;
}

int Controller::GetMode() const {
    return mode_;
}

void Controller::SetMode(const int mode) {
    mode_ = mode;
}

std::vector<Ray> Controller::CastRays() {
    std::vector<Ray> rays;
    rays.reserve(20 * std::size(polygons_));
    for (const auto polygon : polygons_) {
        for (const auto vertex : polygon.GetVertices()) {
            float angle =
                std::atan2(vertex.y() - light_source_.y(), vertex.x() - light_source_.x());
            Ray ray(light_source_, vertex, angle);
            Ray ray1 = ray.Rotate(-0.001);
            Ray ray2 = ray.Rotate(0.001);
            rays.push_back(ray1);
            // rays.push_back(ray);
            rays.push_back(ray2);
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>& rays) {
    for (auto& ray : rays) {
        float min_distance = -1;
        for (auto& poly : polygons_) {
            std::optional<QPointF> ray_poly = poly.IntersectRay(ray);
            if (ray_poly.has_value()) {
                if (min_distance == -1 ||
                    min_distance > sqrt(
                                       (ray.GetBegin().x() - ray_poly.value().x()) *
                                           (ray.GetBegin().x() - ray_poly.value().x()) +
                                       (ray.GetBegin().y() - ray_poly.value().y()) *
                                           (ray.GetBegin().y() - ray_poly.value().y()))) {
                    ray.SetEnd(ray_poly.value());
                    min_distance = sqrt(
                        (ray.GetBegin().x() - ray_poly.value().x()) *
                            (ray.GetBegin().x() - ray_poly.value().x()) +
                        (ray.GetBegin().y() - ray_poly.value().y()) *
                            (ray.GetBegin().y() - ray_poly.value().y()));
                }
            }
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>& rays) {
    std::vector<Ray> filtered;
    filtered.reserve(std::size(rays));
    filtered.push_back(rays.front());

    std::ranges::sort(rays, [](const Ray& a, const Ray& b) { return a.GetAngle() < b.GetAngle(); });

    for (size_t i = 1; i < std::size(rays); i++) {
        const Ray& current = rays[i];
        const Ray& last_unique = filtered.back();

        QPointF current_end = current.GetEnd();
        QPointF lastEnd = last_unique.GetEnd();
        const float dx = current_end.x() - lastEnd.x();
        const float dy = current_end.y() - lastEnd.y();

        if (const float distance = std::sqrt((dx * dx) + (dy * dy)); distance >= 5) {
            filtered.push_back(current);
        }
    }

    rays = std::move(filtered);
}

Polygon Controller::CreateLightArea() {
    light_area_ = Polygon();
    std::vector<Ray> cast_rays = CastRays();

    IntersectRays(cast_rays);
    std::ranges::sort(
        cast_rays, [](const Ray& a, const Ray& b) { return a.GetAngle() < b.GetAngle(); });

    for (const auto& ray : cast_rays) {
        light_area_.AddVertex(ray.GetEnd());
    }
    return light_area_;
}

void Controller::UpdateBorderRect(const QSize& size) {
    std::vector<QPointF> newBorder = {
        QPointF(0, 0),
        QPointF(size.width(), 0),
        QPointF(size.width(), size.height()),
        QPointF(0, size.height())
    };
    polygons_[0] = Polygon(newBorder);
}