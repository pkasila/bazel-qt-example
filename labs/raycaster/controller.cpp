#include "controller.h"
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Controller::Controller() : light_source_(QPoint(400, 300)) {
    InitializeBoundaryPolygon();
}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddVertexToLastPolygon(const QPoint& new_vertex) {
    if (polygons_.empty()) {
        polygons_.emplace_back(std::vector<QPoint>{new_vertex});
    } else {
        polygons_.back().AddVertex(new_vertex);
    }
}

void Controller::UpdateLastPolygon(const QPoint& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().UpdateLastVertex(new_vertex);
    }
}

std::vector<Ray> Controller::CastRays() {
    std::vector<Ray> rays;
    
    const int num_rays = 360;
    const double angle_step = 2 * M_PI / num_rays;
    const double max_distance = 2000;
    
    for (int i = 0; i < num_rays; ++i) {
        double angle = i * angle_step;
        double end_x = light_source_.x() + std::cos(angle) * max_distance;
        double end_y = light_source_.y() + std::sin(angle) * max_distance;
        QPoint end_point(static_cast<int>(end_x), static_cast<int>(end_y));
        
        Ray ray(light_source_, end_point, angle);
        rays.push_back(ray);
    }

    for (const auto& polygon : polygons_) {
        const auto& vertices = polygon.getVertices();
        for (const auto& vertex : vertices) {
            if (&polygon == &polygons_[0]) continue;

            Ray ray(light_source_, vertex, 0);
            rays.push_back(ray);

            rays.push_back(ray.Rotate(0.0001));
            rays.push_back(ray.Rotate(-0.0001));
        }
    }
    
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) {
    for (auto& ray : *rays) {
        QPoint ray_begin = ray.getBegin();
        double min_distance = std::pow(ray.getEnd().x() - ray_begin.x(), 2) + 
                             std::pow(ray.getEnd().y() - ray_begin.y(), 2);
        
        for (const auto& polygon : polygons_) {
            auto intersection = polygon.IntersectRay(ray);
            if (intersection) {
                double distance = std::pow(intersection->x() - ray_begin.x(), 2) + 
                                 std::pow(intersection->y() - ray_begin.y(), 2);
                if (distance < min_distance) {
                    min_distance = distance;
                    ray.setEnd(*intersection);
                }
            }
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) {
    if (rays->size() < 2) return;

    std::sort(rays->begin(), rays->end(), [this](const Ray& a, const Ray& b) {
        return CalculateRayAngle(a) < CalculateRayAngle(b);
    });

    std::vector<Ray> filtered_rays;
    const double threshold = 2.0;
    
    for (const auto& ray : *rays) {
        bool is_close = false;

        const int check_count = std::min(5, static_cast<int>(filtered_rays.size()));
        for (int i = static_cast<int>(filtered_rays.size()) - check_count; i < filtered_rays.size(); ++i) {
            if (i < 0) {
                continue;
            }
            
            const auto& existing_ray = filtered_rays[i];
            double distance = std::pow(ray.getEnd().x() - existing_ray.getEnd().x(), 2) + 
                             std::pow(ray.getEnd().y() - existing_ray.getEnd().y(), 2);
            if (distance < threshold) {
                is_close = true;
                break;
            }
        }
        
        if (!is_close) {
            filtered_rays.push_back(ray);
        }
    }
    
    *rays = filtered_rays;
}

Polygon Controller::CreateLightArea() {
    auto rays = CastRays();
    IntersectRays(&rays);
    RemoveAdjacentRays(&rays);

    std::sort(rays.begin(), rays.end(), [this](const Ray& a, const Ray& b) {
        return CalculateRayAngle(a) < CalculateRayAngle(b);
    });

    std::vector<QPoint> vertices;
    for (const auto& ray : rays) {
        vertices.push_back(ray.getEnd());
    }
    
    return Polygon(vertices);
}

std::vector<Polygon> Controller::CreateLightAreas() {
    std::vector<Polygon> light_areas;
    
    for (const auto& light_source : light_sources_) {
        QPoint original_light = light_source_;
        light_source_ = light_source;

        Polygon light_area = CreateLightArea();
        light_areas.push_back(light_area);

        light_source_ = original_light;
    }
    
    return light_areas;
}

void Controller::InitializeBoundaryPolygon() {
    std::vector<QPoint> boundary_vertices = {
        QPoint(-1000, -1000),
        QPoint(2000, -1000),
        QPoint(2000, 2000),
        QPoint(-1000, 2000)
    };
    polygons_.emplace_back(boundary_vertices);
}

double Controller::CalculateRayAngle(const Ray& ray) const {
    QPoint direction = ray.getEnd() - ray.getBegin();
    return std::atan2(direction.y(), direction.x());
}
