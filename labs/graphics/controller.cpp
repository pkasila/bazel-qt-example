#include "controller.h"

#include <QLineF>
#include <cmath>
#include <set>

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polys_;
}

std::vector<Polygon>& Controller::GetPolygons() {
    return polys_;
}

void Controller::AddPolygon(const Polygon& poly) {
    polys_.push_back(poly);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_ver) {
    if (!polys_.empty()) {
        polys_.back().AddVertex(new_ver);
    }
}

void Controller::UpdateLastPolygon(const QPointF& new_ver) {
    if (!polys_.empty()) {
        polys_.back().UpdateLastVertex(new_ver);
    }
}

const std::vector<QPointF>& Controller::GetLightSources() const {
    return lights_;
}

void Controller::SetLightSources(const std::vector<QPointF>& lights) {
    lights_ = lights;
}

std::vector<Ray> Controller::CastRays(const QPointF& point, const QRectF& bound) const {
    std::vector<Ray> rays;
    const double eps = 0.0001;
    std::set<double> unique_angles;
    for (const auto& poly : polys_) {
        for (const auto& ver : poly.GetVertices()) {
            QPointF dir = ver - point;
            double angle = atan2(dir.y(), dir.x());
            unique_angles.insert(angle - eps);
            unique_angles.insert(angle);
            unique_angles.insert(angle + eps);
        }
    }
    QPointF corners[4] = {
      bound.topLeft(), bound.topRight(), bound.bottomRight(), bound.bottomLeft()};
    for (const auto& corner : corners) {
        unique_angles.insert(atan2(corner.y() - point.y(), corner.x() - point.x()));
    }
    for (double angle : unique_angles) {
        QPointF end =
            point + QPointF(cos(angle) * bound.width() * 2, sin(angle) * bound.height() * 2);
        rays.emplace_back(point, end, angle);
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) {
    for (auto& ray : *rays) {
        QPointF end = ray.GetEnd();
        double min_dist = QLineF(ray.GetBegin(), end).length();

        for (const auto& poly : polys_) {
            auto intersec = poly.IntersectRay(ray);
            if (intersec) {
                double dist = QLineF(ray.GetBegin(), *intersec).length();
                if (dist < min_dist) {
                    min_dist = dist;
                    end = *intersec;
                }
            }
        }

        ray.SetEnd(end);
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) {
    if (rays->empty()) {
        return;
    }
    std::sort(rays->begin(), rays->end(), [](const Ray& a, const Ray& b) {
        return a.GetAngle() < b.GetAngle();
    });

    const double eps = 0.001;
    auto new_end = rays->begin();
    for (auto it = rays->begin(); it != rays->end(); ++it) {
        if (it == rays->begin() || QLineF(it->GetEnd(), (new_end - 1)->GetEnd()).length() > eps) {
            *new_end++ = *it;
        }
    }
    rays->erase(new_end, rays->end());
}

Polygon Controller::CreateLightArea(const QPointF& point, const QRectF& bound) const {
    auto rays = CastRays(point, bound);
    std::vector<Ray> intersec_rays = rays;
    for (auto& ray : intersec_rays) {
        QPointF closest_end = ray.GetEnd();
        double min_dist = QLineF(ray.GetBegin(), closest_end).length();

        for (const auto& poly : polys_) {
            auto intersec = poly.IntersectRay(ray);
            if (intersec) {
                double dist = QLineF(ray.GetBegin(), *intersec).length();
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_end = *intersec;
                }
            }
        }
        ray.SetEnd(closest_end);
    }

    std::sort(intersec_rays.begin(), intersec_rays.end(), [](const Ray& a, const Ray& b) {
        return a.GetAngle() < b.GetAngle();
    });
    std::vector<QPointF> vertices;
    vertices.push_back(point);
    for (const auto& ray : intersec_rays) {
        vertices.push_back(ray.GetEnd());
    }
    if (!intersec_rays.empty()) {
        vertices.push_back(intersec_rays.front().GetEnd());
    }

    return Polygon(vertices);
}
