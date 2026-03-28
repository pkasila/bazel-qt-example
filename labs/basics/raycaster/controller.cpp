#include "controller.h"
#include <algorithm>
#include <cmath>
#include <QLineF>

Controller::Controller() {}

const std::vector<Polygon>& Controller::getPolygons() const {
    return polygons_;
}

void Controller::addPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
}

void Controller::addVertexToLastPolygon(const QPointF& vertex) {
    if (!polygons_.empty()) {
        polygons_.back().addVertex(vertex);
    }
}

void Controller::updateLastPolygon(const QPointF& newVertex) {
    if (!polygons_.empty()) {
        polygons_.back().updateLastVertex(newVertex);
    }
}

void Controller::setLightSource(const QPointF& lightSource) {
    lightSource_ = lightSource;
}

QPointF Controller::getLightSource() const {
    return lightSource_;
}

std::vector<Ray> Controller::castRays() const {
    std::vector<Ray> rays;
    double epsilon = 0.01;

    for (const auto& polygon : polygons_) {
        for (const QPointF& vertex : polygon.getVertices()) {
            QPointF direction = vertex - lightSource_;
            double angle = std::atan2(direction.y(), direction.x());
            Ray ray(lightSource_, vertex, angle);
            rays.push_back(ray);
            rays.push_back(ray.rotate(-epsilon));
            rays.push_back(ray.rotate(epsilon));
        }
    }
    return rays;
}

void Controller::intersectRays(std::vector<Ray>& rays) const {
    for (auto& ray : rays) {
        double minDist = QLineF(ray.getBegin(), ray.getEnd()).length();
        QPointF closest = ray.getEnd();

        for (const auto& polygon : polygons_) {
            auto intersection = polygon.intersectRay(ray);
            if (intersection) {
                double dist = QLineF(ray.getBegin(), *intersection).length();
                if (dist < minDist) {
                    minDist = dist;
                    closest = *intersection;
                }
            }
        }
        ray.setEnd(closest);
    }
}

void Controller::removeAdjacentRays(std::vector<Ray>& rays) const {
    std::vector<Ray> filteredRays;
    double epsilon = 0.5;

    for (const auto& ray : rays) {
        bool duplicate = false;
        for (const auto& fRay : filteredRays) {
            if (QLineF(ray.getEnd(), fRay.getEnd()).length() < epsilon) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) {
            filteredRays.push_back(ray);
        }
    }
    rays = filteredRays;
}

Polygon Controller::createLightArea() const {
    auto rays = castRays();
    intersectRays(rays);
    removeAdjacentRays(rays);

    std::sort(rays.begin(), rays.end(),
              [](const Ray& a, const Ray& b) { return a.getAngle() < b.getAngle(); });

    std::vector<QPointF> lightPoints;
    for (const auto& ray : rays) {
        lightPoints.push_back(ray.getEnd());
    }
    return Polygon(lightPoints);
}
