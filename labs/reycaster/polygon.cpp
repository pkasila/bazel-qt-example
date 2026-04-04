#include "polygon.h"
#include "ray.h"
#include <algorithm>
#include <limits>
#include <cmath>

Polygon::Polygon(const std::vector<QPointF>& vertices) 
    : vertices_(vertices) {
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (!vertices_.empty()) {
        vertices_.back() = new_vertex;
    }
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (vertices_.size() < 2) {
        return std::nullopt;
    }
    
    QPointF closest_intersection;
    double min_distance = std::numeric_limits<double>::max();
    bool found_intersection = false;
    
    // Проверяем пересечение с каждым ребром многоугольника
    for (size_t i = 0; i < vertices_.size(); ++i) {
        size_t next_i = (i + 1) % vertices_.size();
        const QPointF& p1 = vertices_[i];
        const QPointF& p2 = vertices_[next_i];
        
        auto intersection = IntersectSegment(p1, p2, ray);
        if (intersection) {
            double dist = Distance(ray.getBegin(), *intersection);
            if (dist < min_distance) {
                min_distance = dist;
                closest_intersection = *intersection;
                found_intersection = true;
            }
        }
    }
    
    if (found_intersection) {
        return closest_intersection;
    }
    
    return std::nullopt;
}

std::optional<QPointF> Polygon::IntersectSegment(const QPointF& p1, const QPointF& p2, 
                                               const Ray& ray) const {
    // Вектор направления луча
    double ray_dx = ray.getEnd().x() - ray.getBegin().x();
    double ray_dy = ray.getEnd().y() - ray.getBegin().y();
    
    // Вектор отрезка
    double seg_dx = p2.x() - p1.x();
    double seg_dy = p2.y() - p1.y();
    
    // Решаем систему уравнений для нахождения пересечения
    double denominator = ray_dx * seg_dy - ray_dy * seg_dx;
    
    if (std::abs(denominator) < 1e-10) {
        return std::nullopt; // Параллельны или совпадают
    }
    
    double t = ((p1.x() - ray.getBegin().x()) * seg_dy - 
                (p1.y() - ray.getBegin().y()) * seg_dx) / denominator;
    
    double u = ((p1.x() - ray.getBegin().x()) * ray_dy - 
                (p1.y() - ray.getBegin().y()) * ray_dx) / denominator;
    
    if (t >= 0 && u >= 0 && u <= 1) {
        // Пересечение найдено
        double intersect_x = ray.getBegin().x() + t * ray_dx;
        double intersect_y = ray.getBegin().y() + t * ray_dy;
        return QPointF(intersect_x, intersect_y);
    }
    
    return std::nullopt;
}

double Polygon::Distance(const QPointF& p1, const QPointF& p2) const {
    double dx = p1.x() - p2.x();
    double dy = p1.y() - p2.y();
    return std::sqrt(dx * dx + dy * dy);
}
