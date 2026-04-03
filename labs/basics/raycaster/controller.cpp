#include "controller.h"

#include <algorithm>
#include <optional>

Controller::Controller() {
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

std::vector<Polygon>& Controller::GetPolygons() {
    return polygons_;
}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

bool Controller::HasPolygons() const {
    return !polygons_.empty();
}

void Controller::SetLightSource(const QPointF& new_source) {
    light_source_ = new_source;
}

const QPointF& Controller::GetLightSource() const {
    return light_source_;
}

bool Controller::IsPointInsidePolygon(const QPointF& point, const std::vector<QPointF>& polygon) {
    int crossings = 0;
    size_t count = polygon.size();
    for (size_t i = 0; i < count; ++i) {
        QPointF a = polygon[i];
        QPointF b = polygon[(i + 1) % count];

        if (((a.y() > point.y()) != (b.y() > point.y())) &&
            (point.x() < (b.x() - a.x()) * (point.y() - a.y()) / (b.y() - a.y()) + a.x())) {
            crossings++;
        }
    }

    return (crossings % 2) == 1;
}

std::vector<Ray> Controller::CastRays(const QPointF& light_s) {
    std::vector<Ray> allRays;
    for (const Polygon& poly : polygons_) {
        std::vector<QPointF> vertices = poly.GetVertices();
        for (const QPointF& pEnd : vertices) {
            allRays.push_back(Ray(light_s, pEnd).Rotate(0.0001));
            allRays.push_back(Ray(light_s, pEnd).Rotate(-0.0001));
            allRays.push_back(Ray(light_s, pEnd));
        }
    }
    return allRays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) {
    for (Polygon& poly : polygons_) {
        for (Ray& ray : *rays) {
            std::optional<QPointF> inter = poly.IntersectRay(ray);
            if (inter != std::nullopt) {
                double d1 = std::hypot(
                    (ray.GetBegin().x() - ray.GetEnd().x()),
                    (ray.GetBegin().y() - ray.GetEnd().y()));

                double d2 = std::hypot(
                    (ray.GetBegin().x() - inter->x()), (ray.GetBegin().y() - inter->y()));

                if (d2 < d1) {
                    ray.SetEnd(inter.value());
                }
            }
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) {
    std::sort(rays->begin(), rays->end(), [](const Ray& a, const Ray& b) {
        return a.GetAngle() < b.GetAngle();
    });

    auto it = rays->begin();
    while (it != rays->end() - 1) {
        const QPointF& end1 = it->GetEnd();
        const QPointF& end2 = (it + 1)->GetEnd();

        double dist = std::hypot(end2.x() - end1.x(), end2.y() - end1.y());

        if (dist < 0.001) {
            it = rays->erase(it + 1);
        } else {
            ++it;
        }
    }
}

std::vector<Polygon> Controller::CreateLightArea() {
    std::vector<Polygon> polys;
    for (const QPointF& l : light_sources_shades) {
        std::vector<Ray> rays = CastRays(l);
        IntersectRays(&rays);
        RemoveAdjacentRays(&rays);
        std::vector<QPointF> vertices_light;
        for (const Ray& ray : rays) {
            vertices_light.push_back(ray.GetEnd());
        }
        polys.push_back(Polygon(vertices_light));
    }
    return polys;
}

void Controller::GenerateShades() {
    light_sources_shades.clear();
    light_sources_shades.push_back(light_source_);

    for (int i = 0; i < 4; i++) {
        light_sources_shades.push_back(QPointF(
            light_source_.x() + 6.0f * cos(2 * M_PI * i / 4),
            light_source_.y() + 6.0f * sin(2 * M_PI * i / 4)));
    }
}
