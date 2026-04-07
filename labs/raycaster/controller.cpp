#include "controller.h"
#include <algorithm>
#include <cmath>

std::vector<Polygon>& Controller::GetPolygons() {
    return polygons_;
}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& a) {
    polygons_.push_back(a);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (polygons_.empty()) {
        return;
    }
    polygons_.back().AddVertex(new_vertex);
    std::vector<QPointF> vertices = polygons_.back().GetVertices();
    if (vertices.size() >= 2) {
        for (size_t i = 0; i + 1 < vertices.size() - 1; i++) {
            for (size_t j = i + 2; j + 1 < vertices.size(); j++) {
                if (j == i + 1) {
                    continue;
                }
                if (Polygon::LineIntersection(
                        vertices[i], vertices[i + 1], vertices[j], vertices[j + 1])) {
                    polygons_.back().DeleteLastVertex();
                    return;
                }
            }
            if (i == 0 || i + 1 == vertices.size() - 1) {
                continue;
            }
            if (Polygon::LineIntersection(
                    vertices[i], vertices[i + 1], vertices.back(), vertices[0])) {
                polygons_.back().DeleteLastVertex();
                return;
            }
        }
    }
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().UpdateLastVertex(new_vertex);
    }
}

QPointF Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF& new_light_source) {
    light_source_ = new_light_source;
}

std::vector<Ray> Controller::CastRays() const {
    std::vector<Ray> rays;
    for (const auto& polygon : polygons_) {
        for (const auto& vertex : polygon.GetVertices()) {
            int kMaxRayLength = 1488;
            constexpr double kAngleIncrement = 0.0001;
            const double angle = AngleBetween(light_source_, vertex);
            rays.emplace_back(light_source_, vertex, angle);

            const double dx = cosl(angle) * kMaxRayLength;
            const double dy = -sinl(angle) * kMaxRayLength;
            const QPointF end_point = light_source_ + QPointF(dx, dy);

            const Ray main_ray(light_source_, end_point, angle);
            const Ray left_ray = main_ray.Rotate(-kAngleIncrement);
            const Ray right_ray = main_ray.Rotate(kAngleIncrement);

            auto process_ray = [this](const Ray& ray) {
                double min_dist = std::numeric_limits<double>::max();
                QPointF near_intersec = ray.GetEnd();

                for (const auto& poly : polygons_) {
                    auto intersec = poly.IntersectRay(ray);
                    if (intersec) {
                        const double dist = Distance(light_source_, *intersec);
                        if (dist < min_dist) {
                            min_dist = dist;
                            near_intersec = *intersec;
                        }
                    }
                }
                return Ray(ray.GetBegin(), near_intersec, ray.GetAngle());
            };

            rays.push_back(process_ray(left_ray));
            rays.push_back(process_ray(right_ray));
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    for (auto& ray : *rays) {
        double min_dist = Distance(ray.GetBegin(), ray.GetEnd());
        QPointF near_intersec = ray.GetEnd();

        for (const auto& polygon : polygons_) {
            auto intersec = polygon.IntersectRay(ray);
            if (intersec) {
                const double dist = Distance(ray.GetBegin(), *intersec);
                if (dist < min_dist) {
                    min_dist = dist;
                    near_intersec = *intersec;
                }
            }
        }

        ray.SetEnd(near_intersec);
    }
}

double Controller::AngleBetween(const QPointF& center, const QPointF& point) {
    auto normalize_angle = [](double angle) {
        if (angle < 0) {
            angle += 2 * M_PI;
        }
        return angle;
    };
    return normalize_angle(atan2(center.y() - point.y(), point.x() - center.x()));
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    if (rays->empty()) {
        return;
    }

    std::ranges::sort(*rays, [this](const Ray& a, const Ray& b) { return a.GetAngle() < b.GetAngle(); });

    std::vector<Ray> new_rays;
    new_rays.push_back(rays->front());

    for (size_t i = 1; i < rays->size(); ++i) {
        const Ray& prev = new_rays.back();
        const Ray& current = (*rays)[i];
        if ((Distance(current.GetEnd(), prev.GetEnd()) > 1.0)) {
            new_rays.push_back(current);
        }
    }
    *rays = std::move(new_rays);
}

Polygon Controller::CreateLightArea() const {
    auto rays = CastRays();
    IntersectRays(&rays);
    RemoveAdjacentRays(&rays);

    if (rays.empty()) {
        return Polygon({light_source_});
    }

    std::ranges::sort(rays, [this](const Ray& a, const Ray& b) {
        const double angle_a = AngleBetween(light_source_, a.GetEnd());
        const double angle_b = AngleBetween(light_source_, b.GetEnd());
        return angle_a < angle_b;
    });

    std::vector<QPointF> vertices;

    for (const auto& ray : rays) {
        vertices.push_back(ray.GetEnd());
    }

    if (!rays.empty()) {
        vertices.push_back(rays.front().GetEnd());
    }
    return Polygon(vertices);
}

double Controller::Distance(const QPointF P1, const QPointF P2) {
    const double dx = P1.x() - P2.x();
    const double dy = P1.y() - P2.y();
    return std::hypot(dx, dy);
}



void Controller::AddStaticLight(const QPointF& pos) {
    if (!IsPointInsideAnyPolygon(pos)) {
        static_lights_.push_back(pos);
    }
}

const std::vector<QPointF>& Controller::GetStaticLights() const {
    return static_lights_;
}

bool Controller::IsPointInsideAnyPolygon(const QPointF& point) const {
    for (const auto& poly : polygons_) {
        const auto& vertices = poly.GetVertices();
        if (vertices.size() < 3) {
            continue;
        }

        bool inside = false;
        for (size_t i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
            const QPointF& P1 = vertices[i];
            const QPointF& P2 = vertices[j];

            if (((P1.y() > point.y()) != (P2.y() > point.y())) &&
                (point.x() <
                 (P2.x() - P1.x()) * (point.y() - P1.y()) / (P2.y() - P1.y()) + P1.x())) {
                inside = !inside;
            }
        }
        if (inside) {
            return true;
        }
    }
    return false;
}

bool Controller::IsPositionValid(const QPointF& pos) const {
    if (polygons_.empty()) {
        return false;
    }
    std::vector<QPointF> lights;
    lights.push_back(pos);
    constexpr int kCount = 6;
    for (int i = 0; i < kCount; ++i) {
        constexpr int kRadius = 16;
        const double angle = (2 * M_PI * i / kCount);
        lights.emplace_back(
            lights[0].x() + (kRadius * cosl(angle)), lights[0].y() + (kRadius * sinl(angle)));
    }

    const auto& border = polygons_[0].GetVertices();
    bool res = true;
    for (auto& light : lights) {
        bool inside = false;
        for (size_t i = 0, j = border.size() - 1; i < border.size(); j = i++) {
            if (((border[i].y() > light.y()) != (border[j].y() > light.y())) &&
                (light.x() < (border[j].x() - border[i].x()) * (light.y() - border[i].y()) /
                                     (border[j].y() - border[i].y()) +
                                 border[i].x())) {
                inside = !inside;
            }
        }
        res = res && inside;
    }
    return res;
}
