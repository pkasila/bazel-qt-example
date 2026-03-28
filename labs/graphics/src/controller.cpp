#include "../include/controller.h"

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
        for (size_t i = 0; i + 1 < vertices.size() - 1; ++i) {
            for (size_t j = i + 2; j + 1 < vertices.size(); ++j) {
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
        if (DoesPolygonIntersectOthers(polygons_.back().GetVertices())) {
            polygons_.back().DeleteLastVertex();
        }
    }
}

bool Controller::DoesPolygonIntersectOthers(const std::vector<QPointF>& new_vertices) {
    const std::vector<Polygon> polygons = GetPolygons();
    int count = 0;
    for (const auto& poly : polygons) {
        if (count == 0 || count == polygons.size() - 1) {
            count++;
            continue;
        }
        count++;
        const auto& vertices = poly.GetVertices();
        if (vertices.size() < 2) {
            continue;
        }

        for (size_t i = 0; i + 1 < new_vertices.size(); ++i) {
            for (size_t j = 0; j + 1 < vertices.size(); ++j) {
                if (Polygon::LineIntersection(
                        new_vertices[i], new_vertices[i + 1], vertices[j], vertices[j + 1])) {
                    return true;
                }
                if (Polygon::LineIntersection(
                        new_vertices.back(), new_vertices[0], vertices[j], vertices[j + 1])) {
                    return true;
                }
            }
            if (Polygon::LineIntersection(
                    new_vertices[i], new_vertices[i + 1], vertices.back(), vertices[0])) {
                return true;
            }
        }
    }
    return false;
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) {
        //size_t index = polygons_.size() - 1;
        //polygons_.back().DeleteLastVertex();
        //std::vector<QPointF> vertices = polygons_.back().GetVertices();
        //AddVertexToLastPolygon(new_vertex);
        //if (polygons_.back().GetVertices().size() < index) {
        //    for (size_t i = 0; i < polygons_.back().GetVertices().size(); ++i) {
        //        polygons_.back().DeleteLastVertex();
        //    }
        //    for (size_t i = 0; i < index; ++i) {
        //        AddVertexToLastPolygon(vertices[i]);
        //    }
        //}
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
            constexpr double kMaxRayLength = 10000.0;
            // Вместо того чтобы брать значение диагонали приложения, использую большую
            // константу (я ленивый) )
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
                QPointF closest_intersection = ray.GetEnd();

                for (const auto& poly : polygons_) {
                    auto intersection = poly.IntersectRay(ray);
                    if (intersection) {
                        const double dist = Distance(light_source_, *intersection);
                        if (dist < min_dist) {
                            min_dist = dist;
                            closest_intersection = *intersection;
                        }
                    }
                }
                return Ray(ray.GetBegin(), closest_intersection, ray.GetAngle());
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
        QPointF closest_intersection = ray.GetEnd();

        for (const auto& polygon : polygons_) {
            auto intersection = polygon.IntersectRay(ray);
            if (intersection) {
                const double dist = Distance(ray.GetBegin(), *intersection);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_intersection = *intersection;
                }
            }
        }

        ray.SetEnd(closest_intersection);
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

    std::ranges::sort(
        *rays, [this](const Ray& a, const Ray& b) { return a.GetAngle() < b.GetAngle(); });

    std::vector<Ray> new_rays;
    new_rays.push_back(rays->front());

    for (size_t i = 1; i < rays->size(); ++i) {
        const Ray& prev = new_rays.back();
        const Ray& current = (*rays)[i];
        if (std::abs(Distance(current.GetEnd(), prev.GetEnd()) > 1.0)) {
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

double Controller::Distance(const QPointF a, const QPointF b) {
    const double dx = a.x() - b.x();
    const double dy = b.y() - a.y();
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
            const QPointF& p1 = vertices[i];
            const QPointF& p2 = vertices[j];

            if (((p1.y() > point.y()) != (p2.y() > point.y())) &&
                (point.x() <
                 (p2.x() - p1.x()) * (point.y() - p1.y()) / (p2.y() - p1.y()) + p1.x())) {
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
        const double angle = 2 * M_PI * i / kCount;
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