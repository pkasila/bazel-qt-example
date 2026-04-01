#include "controller.h"
#include <algorithm>
#include <cmath>

Controller::Controller() : light_source_(0, 0) {
    light_sources_.resize(7);
    const double radius = 20.0;
    for (int i = 0; i < 7; ++i) {
        double angle = 2.0 * M_PI * i / 7.0;
        light_sources_[i] = light_source_ + QPointF(radius * std::cos(angle), radius * std::sin(angle));
    }
}

void Controller::setLightSource(const QPointF& source) {
    light_source_ = source;
    light_sources_.resize(7);
    const double radius = 20.0;
    for (int i = 0; i < 7; ++i) {
        double angle = 2.0 * M_PI * i / 7.0;
        light_sources_[i] = light_source_ + QPointF(radius * std::cos(angle), radius * std::sin(angle));
    }
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().AddVertex(new_vertex);
    }
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().UpdateLastVertex(new_vertex);
    }
}

std::vector<Ray> Controller::CastRays() {
    std::vector<Ray> rays;
    for (const QPointF& source : light_sources_) {
        for (const Polygon& poly : polygons_) {
            for (const QPointF& vertex : poly.getVertices()) {
                double dx = vertex.x() - source.x();
                double dy = vertex.y() - source.y();
                double angle = std::atan2(dy, dx);

                rays.push_back(Ray(source, vertex, angle));
                rays.push_back(Ray(source, vertex, angle).Rotate(0.0001));
                rays.push_back(Ray(source, vertex, angle).Rotate(-0.0001));
            }
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) {
    for (Ray& ray : *rays) {
        QPointF rayBegin = ray.getBegin();
        QPointF rayEnd = ray.getEnd();
        double originalLength = std::hypot(rayEnd.x() - rayBegin.x(), rayEnd.y() - rayBegin.y());

        std::optional<QPointF> closestIntersection;
        double minDistance = originalLength;

        for (const Polygon& poly : polygons_) {
            std::optional<QPointF> intersection = poly.IntersectRay(ray);
            if (intersection) {
                double distance = std::hypot(intersection->x() - rayBegin.x(), intersection->y() - rayBegin.y());
                if (distance < minDistance) {
                    minDistance = distance;
                    closestIntersection = intersection;
                }
            }
        }

        if (closestIntersection) {
            ray.setEnd(*closestIntersection);
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) {
    if (rays->empty()) return;

    std::sort(rays->begin(), rays->end(), [](const Ray& a, const Ray& b) {
        return a.getAngle() < b.getAngle();
    });

    std::vector<Ray> filteredRays;
    filteredRays.push_back((*rays)[0]);

    const double threshold = 2.0;
    for (size_t i = 1; i < rays->size(); ++i) {
        QPointF lastEnd = filteredRays.back().getEnd();
        QPointF currentEnd = (*rays)[i].getEnd();
        double distance = std::hypot(currentEnd.x() - lastEnd.x(), currentEnd.y() - lastEnd.y());

        if (distance > threshold) {
            filteredRays.push_back((*rays)[i]);
        }
    }

    *rays = filteredRays;
}

Polygon Controller::CreateLightArea(const QPointF& source) {
    std::vector<Ray> rays;
    for (const Polygon& poly : polygons_) {
        for (const QPointF& vertex : poly.getVertices()) {
            double dx = vertex.x() - source.x();
            double dy = vertex.y() - source.y();
            double angle = std::atan2(dy, dx);

            rays.push_back(Ray(source, vertex, angle));
            rays.push_back(Ray(source, vertex, angle).Rotate(0.0001));
            rays.push_back(Ray(source, vertex, angle).Rotate(-0.0001));
        }
    }

    IntersectRays(&rays);

    std::sort(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) {
        return a.getAngle() < b.getAngle();
    });

    RemoveAdjacentRays(&rays);

    std::vector<QPointF> vertices;
    for (const Ray& ray : rays) {
        vertices.push_back(ray.getEnd());
    }

    return Polygon(vertices);
}