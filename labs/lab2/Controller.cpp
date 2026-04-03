#include "Controller.h"
#include <algorithm>
#include <cmath>

std::vector<Ray> Controller::CastRays() {
    std::vector<Ray> rays;
    for (const auto& poly : m_polygons) {
        for (const auto& v : poly.getVertices()) {
            double a = std::atan2(v.y() - m_light_source.y(), v.x() - m_light_source.x());
            Ray r(m_light_source, QPointF(m_light_source.x() + std::cos(a)*2000, m_light_source.y() + std::sin(a)*2000), a);
            rays.push_back(r);
            rays.push_back(r.Rotate(0.0001));
            rays.push_back(r.Rotate(-0.0001));
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) {
    for (auto& ray : *rays) {
        QPointF best = ray.getEnd();
        double min_d = std::hypot(best.x() - m_light_source.x(), best.y() - m_light_source.y());
        for (const auto& poly : m_polygons) {
            if (auto hit = poly.IntersectRay(ray)) {
                double d = std::hypot(hit->x() - m_light_source.x(), hit->y() - m_light_source.y());
                if (d < min_d) { min_d = d; best = *hit; }
            }
        }
        ray.setEnd(best);
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) {
    rays->erase(std::unique(rays->begin(), rays->end(), [](const Ray& a, const Ray& b){
        return std::hypot(a.getEnd().x() - b.getEnd().x(), a.getEnd().y() - b.getEnd().y()) < 0.1;
    }), rays->end());
}

Polygon Controller::CreateLightArea() {
    auto rays = CastRays();
    IntersectRays(&rays);
    std::sort(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) { return a.getAngle() < b.getAngle(); });
    RemoveAdjacentRays(&rays);
    std::vector<QPointF> verts;
    for(const auto& r : rays) verts.push_back(r.getEnd());
    return Polygon(verts);
}