#include "controller.h"
#include <algorithm>
#include <cmath>
#include <limits>

Controller::Controller(int width, int height) : width_(width), height_(height), circle_radius_(15.0) {
    light_source_ = QPoint(width / 2, height / 2);
    updateMultipleLightsCenter(QPoint(width / 2, height / 2));
}

QPoint Controller::ClosestPointOnSegment(const QPoint& p, const QPoint& a, const QPoint& b) const {
    QPoint ab = b - a;
    QPoint ap = p - a;
    
    double t = (ap.x() * ab.x() + ap.y() * ab.y()) / (ab.x() * ab.x() + ab.y() * ab.y() + 1e-9);
    t = std::max(0.0, std::min(1.0, t));
    
    return QPoint(
        a.x() + static_cast<int>(t * ab.x()),
        a.y() + static_cast<int>(t * ab.y())
    );
}

QPoint Controller::FindNearestPointOutsidePolygon(const QPoint& point) const {
    QPoint result = point;
    double minDistance = std::numeric_limits<double>::max();
    
    for (const auto& polygon : polygons_) {
        const auto& vertices = polygon.getVertices();
        for (size_t i = 0; i < vertices.size(); i++) {
            const QPoint& a = vertices[i];
            const QPoint& b = vertices[(i + 1) % vertices.size()];
            QPoint closest = ClosestPointOnSegment(point, a, b);
            double distance = std::hypot(closest.x() - point.x(), closest.y() - point.y());
            
            if (distance < minDistance) {
                minDistance = distance;
                result = closest;
            }
        }
    }
    if (result != point) {
        QPoint direction = point - result;
        double len = std::hypot(direction.x(), direction.y());
        if (len > 1e-6) {
            result = QPoint(
                result.x() + static_cast<int>(direction.x() / len * 2),
                result.y() + static_cast<int>(direction.y() / len * 2)
            );
        }
    }
    
    return result;
}

void Controller::AddPolygon(const Polygon& polygon) {
    if (IsPolygonValid(polygon)) {
        polygons_.push_back(polygon);
    }
}

void Controller::AddVertexToLastPolygon(const QPoint& new_vertex) {
    std::vector<QPoint> tempVertices = currentPolygonVertices_;
    tempVertices.push_back(new_vertex);
    
    if (tempVertices.size() >= 3) {
        Polygon tempPolygon(tempVertices);
        if (!IsPolygonValid(tempPolygon)) {
            return;
        }
    }
    
    currentPolygonVertices_.push_back(new_vertex);
}

void Controller::UpdateLastPolygon(const QPoint& new_vertex) {
    if (!currentPolygonVertices_.empty()) {
        std::vector<QPoint> tempVertices = currentPolygonVertices_;
        tempVertices.back() = new_vertex;
        
        if (tempVertices.size() >= 3) {
            Polygon tempPolygon(tempVertices);
            if (!IsPolygonValid(tempPolygon)) {
                return;
            }
        }
        currentPolygonVertices_.back() = new_vertex;
    }
}

void Controller::FinishCurrentPolygon() {
    if (currentPolygonVertices_.size() >= 3) {
        Polygon newPolygon(currentPolygonVertices_);
        if (IsPolygonValid(newPolygon)) {
            polygons_.push_back(newPolygon);
        }
    }
    currentPolygonVertices_.clear();
}

bool Controller::IsPolygonValid(const Polygon& polygon) const {
    const auto& vertices = polygon.getVertices();
    if (vertices.size() < 3) return false;
    
    for (size_t i = 0; i < vertices.size(); i++) {
        const QPoint& a1 = vertices[i];
        const QPoint& a2 = vertices[(i + 1) % vertices.size()];
        
        for (size_t j = i + 2; j < vertices.size(); j++) {
            const QPoint& b1 = vertices[j];
            const QPoint& b2 = vertices[(j + 1) % vertices.size()];
            if ((i + 1) % vertices.size() == j || (j + 1) % vertices.size() == i) {
                continue;
            }
            
            if (DoSegmentsIntersect(a1, a2, b1, b2)) {
                return false;
            }
        }
    }
    
    for (const auto& existingPolygon : polygons_) {
        const auto& existingVertices = existingPolygon.getVertices();
        for (size_t i = 0; i < vertices.size(); i++) {
            const QPoint& a1 = vertices[i];
            const QPoint& a2 = vertices[(i + 1) % vertices.size()];
            
            for (size_t j = 0; j < existingVertices.size(); j++) {
                const QPoint& b1 = existingVertices[j];
                const QPoint& b2 = existingVertices[(j + 1) % existingVertices.size()];
                
                if (DoSegmentsIntersect(a1, a2, b1, b2)) {
                    return false;
                }
            }
        }
        for (const auto& vertex : vertices) {
            if (IsPointInsidePolygon(vertex, existingPolygon)) {
                return false;
            }
        }
        for (const auto& vertex : existingVertices) {
            if (IsPointInsidePolygon(vertex, polygon)) {
                return false;
            }
        }
    }
    
    return true;
}

bool Controller::DoSegmentsIntersect(const QPoint& a1, const QPoint& a2, const QPoint& b1, const QPoint& b2) const {
    auto intersection = Polygon::GetLineIntersectionStatic(a1, a2, b1, b2);
    return intersection.has_value();
}

bool Controller::IsPointInsidePolygon(const QPoint& point, const Polygon& polygon) const {
    const auto& vertices = polygon.getVertices();
    if (vertices.size() < 3) return false;
    
    bool inside = false;
    for (size_t i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
        if (((vertices[i].y() > point.y()) != (vertices[j].y() > point.y())) &&
            (point.x() < (vertices[j].x() - vertices[i].x()) * (point.y() - vertices[i].y()) /
            (vertices[j].y() - vertices[i].y()) + vertices[i].x())) {
            inside = !inside;
        }
    }
    return inside;
}

bool Controller::IsPointInsideAnyPolygon(const QPoint& point) const {
    for (const auto& polygon : polygons_) {
        if (IsPointInsidePolygon(point, polygon)) {
            return true;
        }
    }
    return false;
}

void Controller::addStaticLight(const QPoint& position) {
    if (!IsPointInsideAnyPolygon(position)) {
        static_lights_.push_back(position);
    }
}

void Controller::removeLastStaticLight() {
    if (!static_lights_.empty()) {
        static_lights_.pop_back();
    }
}

void Controller::updateMultipleLightsCenter(const QPoint& center) {
    QPoint boundedCenter = center;
    boundedCenter.setX(std::max(0, std::min(center.x(), width_)));
    boundedCenter.setY(std::max(0, std::min(center.y(), height_)));
    
    multiple_light_sources_.clear();
    multiple_light_sources_.push_back(boundedCenter);
    
    for (int i = 0; i < 10; i++) {
        double angle = 2 * M_PI * i / 10.0;
        QPoint offset(
            static_cast<int>(circle_radius_ * std::cos(angle)),
            static_cast<int>(circle_radius_ * std::sin(angle))
        );
        QPoint candidate(boundedCenter.x() + offset.x(), boundedCenter.y() + offset.y());
        if (IsPointInsideAnyPolygon(candidate)) {
            candidate = FindNearestPointOutsidePolygon(candidate);
        }
        
        multiple_light_sources_.push_back(candidate);
    }
}

QPoint Controller::IntersectWithBoundary(const QPoint& source, double angle) const {
    double t_x = 1e9;
    double t_y = 1e9;
    
    if (std::cos(angle) > 0) {
        t_x = (width_ - source.x()) / std::cos(angle);
    } else if (std::cos(angle) < 0) {
        t_x = (0 - source.x()) / std::cos(angle);
    }
    
    if (std::sin(angle) > 0) {
        t_y = (height_ - source.y()) / std::sin(angle);
    } else if (std::sin(angle) < 0) {
        t_y = (0 - source.y()) / std::sin(angle);
    }
    
    double t_min = std::min(t_x, t_y);
    if (t_min > 0 && t_min < 1e8) {
        return QPoint(
            source.x() + static_cast<int>(t_min * std::cos(angle)),
            source.y() + static_cast<int>(t_min * std::sin(angle))
        );
    }
    
    return QPoint(source.x() + static_cast<int>(2000 * std::cos(angle)),
                  source.y() + static_cast<int>(2000 * std::sin(angle)));
}

Polygon Controller::CreateLightArea(const QPoint& source) const {
    std::vector<Ray> rays;
    
    for (int i = 0; i < 360; i += 1) {
        double angle = i * M_PI / 180.0;
        QPoint farPoint(
            source.x() + static_cast<int>(2000 * std::cos(angle)),
            source.y() + static_cast<int>(2000 * std::sin(angle))
        );
        
        Ray ray(source, farPoint, angle);
        QPoint closestPoint = farPoint;
        double minDistance = 2000;
        bool hasIntersection = false;
        for (const auto& polygon : polygons_) {
            auto intersection = polygon.IntersectRay(ray);
            if (intersection) {
                hasIntersection = true;
                double distance = std::hypot(
                    intersection->x() - source.x(),
                    intersection->y() - source.y()
                );
                if (distance < minDistance && distance > 1e-6) {
                    minDistance = distance;
                    closestPoint = *intersection;
                }
            }
        }
        if (!hasIntersection) {
            closestPoint = IntersectWithBoundary(source, angle);
        }
        
        ray.setEnd(closestPoint);
        rays.push_back(ray);
    }

    std::sort(rays.begin(), rays.end(),
        [](const Ray& a, const Ray& b) {
            return a.getAngle() < b.getAngle();
        });
    
    std::vector<QPoint> vertices;
    for (const auto& ray : rays) {
        vertices.push_back(ray.getEnd());
    }
    
    vertices.erase(std::unique(vertices.begin(), vertices.end(),
        [](const QPoint& a, const QPoint& b) {
            return std::abs(a.x() - b.x()) < 2 && std::abs(a.y() - b.y()) < 2;
        }), vertices.end());
    
    if (!vertices.empty()) {
        vertices.push_back(vertices.front());
    }
    
    return Polygon(vertices);
}

std::vector<Polygon> Controller::CreateMultipleLightAreas() const {
    std::vector<Polygon> areas;
    for (const auto& source : multiple_light_sources_) {
        areas.push_back(CreateLightArea(source));
    }
    return areas;
}

std::vector<Polygon> Controller::CreateStaticLightAreas() const {
    std::vector<Polygon> areas;
    for (const auto& source : static_lights_) {
        areas.push_back(CreateLightArea(source));
    }
    return areas;
}