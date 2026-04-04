#include "controller.h"
#include <algorithm>
#include <cmath>
#include <limits>

Controller::Controller() {
    // Инициализация источника света по умолчанию
    light_source_ = QPointF(400, 300);
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (polygons_.empty()) {
        polygons_.emplace_back();
    }
    polygons_.back().AddVertex(new_vertex);
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().UpdateLastVertex(new_vertex);
    }
}

std::vector<Ray> Controller::CastRays() {
    std::vector<Ray> rays;
    constexpr double kMaxDist = 10000.0;

    for (const auto& polygon : polygons_) {
        const auto& vertices = polygon.getVertices();
        for (const auto& vertex : vertices) {
            double dx = vertex.x() - light_source_.x();
            double dy = vertex.y() - light_source_.y();
            double angle = std::atan2(dy, dx);

            QPointF far_end(light_source_.x() + kMaxDist * std::cos(angle),
                            light_source_.y() + kMaxDist * std::sin(angle));
            Ray base_ray(light_source_, far_end, angle);
            rays.push_back(base_ray);
            rays.push_back(base_ray.Rotate(-0.0001));
            rays.push_back(base_ray.Rotate(0.0001));
        }
    }

    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) {
    for (auto& ray : *rays) {
        double min_distance = std::numeric_limits<double>::max();
        QPointF closest_point = ray.getEnd();
        bool found_intersection = false;
        
        for (const auto& polygon : polygons_) {
            auto hit = polygon.IntersectRay(ray);
            if (hit.has_value()) {
                double distance = std::sqrt(
                    std::pow(ray.getBegin().x() - hit->x(), 2) +
                    std::pow(ray.getBegin().y() - hit->y(), 2)
                );
                
                if (distance < min_distance) {
                    min_distance = distance;
                    closest_point = *hit;
                    found_intersection = true;
                }
            }
        }
        
        if (found_intersection) {
            ray.setEnd(closest_point);
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) {
    if (rays->empty()) {
        return;
    }

    constexpr double kMinDist = 1.0;

    std::vector<Ray> result;
    result.push_back((*rays)[0]);

    for (size_t i = 1; i < rays->size(); ++i) {
        const QPointF& prev_end = result.back().getEnd();
        const QPointF& cur_end = (*rays)[i].getEnd();
        double dx = cur_end.x() - prev_end.x();
        double dy = cur_end.y() - prev_end.y();
        double dist = dx * dx + dy * dy;

        if (dist > kMinDist * kMinDist) {
            result.push_back((*rays)[i]);
        }
    }

    *rays = std::move(result);
}

Polygon Controller::CreateLightArea() {
    std::vector<Ray> rays = CastRays();
    IntersectRays(&rays);

    std::sort(rays.begin(), rays.end(), [this](const Ray& a, const Ray& b) {
        return CalculateRayAngle(a) < CalculateRayAngle(b);
    });

    RemoveAdjacentRays(&rays);

    std::vector<QPointF> vertices;
    vertices.reserve(rays.size());
    for (const auto& ray : rays) {
        vertices.push_back(ray.getEnd());
    }

    return Polygon(vertices);
}

void Controller::AddLightSource(const QPointF& light_source) {
    light_sources_.push_back(light_source);
}

std::vector<Polygon> Controller::CreateLightAreas() {
    std::vector<Polygon> light_areas;
    
    for (const auto& light_source : light_sources_) {
        // Временно устанавливаем текущий источник света
        QPointF old_source = light_source_;
        light_source_ = light_source;
        
        // Создаем освещенную область для этого источника
        Polygon light_area = CreateLightArea();
        light_areas.push_back(light_area);
        
        // Восстанавливаем старый источник
        light_source_ = old_source;
    }
    
    return light_areas;
}

void Controller::CreateBoundaryPolygon(int width, int height) {
    std::vector<QPointF> boundary_vertices = {
        QPointF(0, 0), QPointF(width - 1, 0), 
        QPointF(width - 1, height - 1), QPointF(0, height - 1)
    };
    
    // Вставляем граничный многоугольник в начало
    polygons_.insert(polygons_.begin(), Polygon(boundary_vertices));
}

double Controller::CalculateRayAngle(const Ray& ray) const {
    double dx = ray.getEnd().x() - ray.getBegin().x();
    double dy = ray.getEnd().y() - ray.getBegin().y();
    return std::atan2(dy, dx);
}
