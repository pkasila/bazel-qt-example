#include "controller.h"

Polygon Controller::createLightArea(QPointF source) {
    lightSource_ = source;
    std::vector<Ray> rays = castRays();
    intersectRays(rays);
    
    std::sort(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) {
        return a.getAngle() < b.getAngle();
    });
    
    removeAdjacentRays(rays);
    
    std::vector<QPointF> lightVertices;
    for (const auto& r : rays) lightVertices.push_back(r.getEnd());
    return Polygon(lightVertices);
}

std::vector<Ray> Controller::castRays() {
    std::vector<Ray> rays;
    for (const auto& poly : polygons_) {
        for (const auto& v : poly.getVertices()) {
            double angle = std::atan2(v.y() - lightSource_.y(), v.x() - lightSource_.x());
            Ray base(lightSource_, v, angle);
            rays.push_back(base);
            rays.push_back(base.rotate(0.0001));
            rays.push_back(base.rotate(-0.0001));
        }
    }
    return rays;
}

void Controller::intersectRays(std::vector<Ray>& rays) {
    for (auto& ray : rays) {
        for (const auto& poly : polygons_) {
            auto hit = poly.intersectRay(ray);
            if (hit) ray.setEnd(*hit);
        }
    }
}

void Controller::removeAdjacentRays(std::vector<Ray>& rays) {
    if (rays.size() < 2) return;
    auto it = std::unique(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) {
        QPointF diff = a.getEnd() - b.getEnd();
        return (std::abs(diff.x()) < 0.1 && std::abs(diff.y()) < 0.1);
    });
    rays.erase(it, rays.end());
}