#include "controller.h"
#include <algorithm>
#include <cmath>

double Controller::Distance(const QPointF& a, const QPointF& b) {
    return std::hypot(a.x() - b.x(), a.y() - b.y());
}

Controller::Controller() {
    polygons_.push_back(Polygon());
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddVertexToLastPolygon(const QPoint& new_vertex) {
    if (polygons_.size() <= 1) return;
    polygons_.back().AddVertex(new_vertex);
}

void Controller::UpdateLastPolygon(const QPoint& new_vertex) {
    if (polygons_.size() <= 1) return;
    polygons_.back().UpdateLastVertex(new_vertex);
}

QPoint Controller::GetLightSource() const {
    return light_sources_.empty() ? QPoint(0, 0) : light_sources_[0].toPoint();
}

void Controller::SetLightSource(const QPoint& pos) {
    SetLightCluster(pos);
}

void Controller::SetLightCluster(const QPointF& center) {
    light_sources_.clear();
    light_sources_.push_back(center);
    light_sources_.push_back(center + QPointF(15.0, 5.0));
    light_sources_.push_back(center - QPointF(10.0, -10.0));
}

void Controller::SetBounds(const QRectF& bounds) {
    if (!polygons_.empty()) {
        polygons_[0] = Polygon({
            QPoint(bounds.left(), bounds.top()),
            QPoint(bounds.right(), bounds.top()),
            QPoint(bounds.right(), bounds.bottom()),
            QPoint(bounds.left(), bounds.bottom())
        });
    }
}

double Controller::VectorAngle(const QPointF& from, const QPointF& to) {
    return std::atan2(to.y() - from.y(), to.x() - from.x());
}

std::vector<Ray> Controller::CastRaysForLight(const QPointF& light) const {
    std::vector<Ray> rays;
    const double epsilon = 0.0001;
    
    const auto& border = polygons_[0].getVertices();
    for (const auto& corner : border) {
        double angle = VectorAngle(light, corner);
        rays.emplace_back(light, corner, angle);
        rays.emplace_back(light, corner, angle + epsilon);
        rays.emplace_back(light, corner, angle - epsilon);
    }
    
    for (size_t i = 1; i < polygons_.size(); ++i) {
        for (const auto& v : polygons_[i].getVertices()) {
            double base_angle = VectorAngle(light, v);
            double dist = Distance(light, v);
            
            rays.emplace_back(light, v, base_angle);
            
            QPointF end_plus(light.x() + dist * std::cos(base_angle + epsilon),
                             light.y() + dist * std::sin(base_angle + epsilon));
            rays.emplace_back(light, end_plus, base_angle + epsilon);
            
            QPointF end_minus(light.x() + dist * std::cos(base_angle - epsilon),
                              light.y() + dist * std::sin(base_angle - epsilon));
            rays.emplace_back(light, end_minus, base_angle - epsilon);
        }
    }
    return rays;
}

std::vector<Ray> Controller::CastRays() const {
    if (light_sources_.empty()) return {};
    return CastRaysForLight(light_sources_[0]);
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    if (!rays) return;
    for (auto& ray : *rays) {
        std::optional<QPointF> closest;
        double min_dist = ray.length() + 10000.0;
        for (const auto& polygon : polygons_) {
            auto intersection = polygon.IntersectRay(ray);
            if (intersection) {
                double dist = Distance(ray.getBegin(), *intersection);
                if (dist < min_dist - 1e-6) {
                    min_dist = dist;
                    closest = *intersection;
                }
            }
        }
        if (closest) ray.setEnd(*closest);
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    if (!rays || rays->size() <= 1) return;
    const double threshold = 2.0;
    std::vector<bool> keep(rays->size(), true);
    
    for (size_t i = 0; i < rays->size(); ++i) {
        if (!keep[i]) continue;
        for (size_t j = i + 1; j < rays->size(); ++j) {
            if (!keep[j]) continue;
            if (Distance((*rays)[i].getEnd(), (*rays)[j].getEnd()) < threshold) {
                keep[j] = false;
            }
        }
    }
    
    size_t write = 0;
    for (size_t read = 0; read < rays->size(); ++read) {
        if (keep[read]) {
            if (write != read) (*rays)[write] = (*rays)[read];
            ++write;
        }
    }
    rays->resize(write);
}

Polygon Controller::CreateLightArea() const {
    if (light_sources_.empty()) return Polygon();
    
    auto rays = CastRays();
    IntersectRays(&rays);
    RemoveAdjacentRays(&rays);
    
    const QPointF& light = light_sources_[0];
    std::vector<std::pair<double, QPointF>> sorted;
    for (const auto& ray : rays) {
        sorted.push_back({VectorAngle(light, ray.getEnd()), ray.getEnd()});
    }
    
    std::sort(sorted.begin(), sorted.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });
    
    std::vector<QPoint> points;
    for (const auto& [angle, pt] : sorted) {
        points.emplace_back(pt.toPoint());
    }
    
    return points.size() >= 3 ? Polygon(points) : Polygon();
}

std::vector<Polygon> Controller::CreateLightAreas() const {
    std::vector<Polygon> result;
    for (const auto& light : light_sources_) {
        auto rays = CastRaysForLight(light);
        IntersectRays(&rays);
        RemoveAdjacentRays(&rays);
        
        std::vector<std::pair<double, QPointF>> sorted;
        for (const auto& ray : rays) {
            sorted.push_back({VectorAngle(light, ray.getEnd()), ray.getEnd()});
        }
        std::sort(sorted.begin(), sorted.end(),
            [](const auto& a, const auto& b) { return a.first < b.first; });
        
        std::vector<QPoint> points;
        for (const auto& [angle, pt] : sorted) {
            points.emplace_back(pt.toPoint());
        }
        if (points.size() >= 3) {
            result.push_back(Polygon(points));
        }
    }
    return result;
}