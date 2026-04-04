#include "controller.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <ranges>

Controller::Controller(int width, int height)
    : width_(width), height_(height), circle_radius_(10.0) {
    AddBoundaryPolygon();
    light_source_ = QPointF(width / 2.0, height / 2.0);
    UpdateMultipleLightsCenter(light_source_);
}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF& vertex) {
    current_polygon_vertices_.push_back(vertex);
}

void Controller::UpdateLastPolygon(const QPointF& vertex) {
    if (!current_polygon_vertices_.empty()) {
        current_polygon_vertices_.back() = vertex;
    }
}

void Controller::FinishCurrentPolygon() {
    if (current_polygon_vertices_.size() > 2) {
        Polygon p(current_polygon_vertices_);
        if (IsPolygonValid(p)) {
            polygons_.emplace_back(p);
        }
    }
    current_polygon_vertices_.clear();
}

QPointF Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF& source) {
    QPointF next_pos = source;
    next_pos.setX(std::clamp(next_pos.x(), 1.0, static_cast<double>(width_) - 1.0));
    next_pos.setY(std::clamp(next_pos.y(), 1.0, static_cast<double>(height_) - 1.0));

    if (IsPointInsideAnyPolygon(next_pos)) {
        Ray path(light_source_, next_pos, 0.0);
        std::optional<QPointF> hit;
        double min_dist_sq = std::numeric_limits<double>::max();

        for (const auto& poly : polygons_) {
            auto intersection = poly.IntersectRay(path);
            if (intersection) {
                double d = (intersection->x() - light_source_.x()) *
                               (intersection->x() - light_source_.x()) +
                           (intersection->y() - light_source_.y()) *
                               (intersection->y() - light_source_.y());
                if (d < min_dist_sq) {
                    min_dist_sq = d;
                    hit = intersection;
                }
            }
        }

        if (hit) {
            double dx = hit->x() - light_source_.x();
            double dy = hit->y() - light_source_.y();
            double len = std::sqrt(dx * dx + dy * dy);
            if (len > 0.5) {
                double ratio = (len - 0.5) / len;
                light_source_ =
                    QPointF(light_source_.x() + dx * ratio, light_source_.y() + dy * ratio);
            }
        }
    } else {
        light_source_ = next_pos;
    }
    UpdateMultipleLightsCenter(light_source_);
}

std::vector<Ray> Controller::CastRays() const {
    return CastRays(light_source_);
}

std::vector<Ray> Controller::CastRays(const QPointF& source) const {
    std::vector<Ray> rays;
    std::vector<double> angles;
    angles.reserve(300);

    auto add_angle = [&](double angle) {
        angles.push_back(angle);
        angles.push_back(angle + 0.0001);
        angles.push_back(angle - 0.0001);
    };

    for (size_t i = 0; i < polygons_.size(); ++i) {
        const auto& vertices = polygons_[i].GetVertices();
        for (size_t j = 0; j < vertices.size(); ++j) {
            QPointF p1 = vertices[j];
            add_angle(std::atan2(p1.y() - source.y(), p1.x() - source.x()));

            if (i == 0) {
                continue;
            }

            QPointF p2 = vertices[(j + 1) % vertices.size()];
            auto check_edge = [&](double limit, bool is_x) {
                double den = is_x ? (p2.x() - p1.x()) : (p2.y() - p1.y());
                if (std::abs(den) < 1e-9) {
                    return;
                }
                double t = (limit - (is_x ? p1.x() : p1.y())) / den;
                if (t > 0 && t < 1) {
                    QPointF hit = p1 + t * (p2 - p1);
                    if (is_x) {
                        if (hit.y() >= 0 && hit.y() <= height_) {
                            add_angle(std::atan2(hit.y() - source.y(), hit.x() - source.x()));
                        }
                    } else {
                        if (hit.x() >= 0 && hit.x() <= width_) {
                            add_angle(std::atan2(hit.y() - source.y(), hit.x() - source.x()));
                        }
                    }
                }
            };
            check_edge(1.0, true);
            check_edge(width_ - 1.0, true);
            check_edge(1.0, false);
            check_edge(height_ - 1.0, false);
        }
    }

    add_angle(std::atan2(1.0 - source.y(), 1.0 - source.x()));
    add_angle(std::atan2(1.0 - source.y(), static_cast<double>(width_) - 1.0 - source.x()));
    add_angle(std::atan2(static_cast<double>(height_) - 1.0 - source.y(), 1.0 - source.x()));
    add_angle(
        std::atan2(
            static_cast<double>(height_) - 1.0 - source.y(),
            static_cast<double>(width_) - 1.0 - source.x()));

    std::ranges::sort(angles);
    auto [first, last] =
        std::ranges::unique(angles, [](double a, double b) { return std::abs(a - b) < 1e-10; });
    angles.erase(first, last);

    double max_len = std::sqrt(width_ * width_ + height_ * height_) * 2.5;
    rays.reserve(angles.size());
    for (double angle : angles) {
        rays.emplace_back(
            source,
            QPointF(source.x() + std::cos(angle) * max_len, source.y() + std::sin(angle) * max_len),
            angle);
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    for (auto& ray : *rays) {
        QPointF closest = ray.GetEnd();
        QPointF start = ray.GetBegin();
        double min_dist_sq = (closest.x() - start.x()) * (closest.x() - start.x()) +
                             (closest.y() - start.y()) * (closest.y() - start.y());

        for (const auto& poly : polygons_) {
            auto hit = poly.IntersectRay(ray);
            if (hit) {
                double dx = hit->x() - start.x();
                double dy = hit->y() - start.y();
                double d_sq = dx * dx + dy * dy;
                if (d_sq < min_dist_sq) {
                    min_dist_sq = d_sq;
                    closest = *hit;
                }
            }
        }
        ray.SetEnd(closest);
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    std::ranges::sort(
        *rays, [](const Ray& a, const Ray& b) { return a.GetAngle() < b.GetAngle(); });

    auto [first, last] = std::ranges::unique(*rays, [](const Ray& a, const Ray& b) {
        double da = std::abs(a.GetAngle() - b.GetAngle());
        if (da > 1e-7) {
            return false;
        }
        double dx = a.GetEnd().x() - b.GetEnd().x();
        double dy = a.GetEnd().y() - b.GetEnd().y();
        return (dx * dx + dy * dy) < 0.1;
    });
    rays->erase(first, last);
}

Polygon Controller::CreateLightArea() const {
    return CreateLightArea(light_source_);
}

Polygon Controller::CreateLightArea(const QPointF& source) const {
    auto rays = CastRays(source);
    IntersectRays(&rays);
    RemoveAdjacentRays(&rays);

    std::vector<QPointF> vertices;
    vertices.reserve(rays.size());
    for (const auto& r : rays) {
        vertices.push_back(r.GetEnd());
    }
    return Polygon(vertices);
}

std::vector<QPointF> Controller::GetMultipleLightSources() const {
    return multiple_light_sources_;
}

void Controller::UpdateMultipleLightsCenter(const QPointF& center) {
    multiple_light_sources_.clear();
    const int count = 16;
    multiple_light_sources_.reserve(count);

    for (int i = 0; i < count; ++i) {
        double angle = 2.0 * std::numbers::pi * i / count;
        QPointF next_p(
            center.x() + circle_radius_ * std::cos(angle),
            center.y() + circle_radius_ * std::sin(angle));

        next_p.setX(std::clamp(next_p.x(), 1.0, static_cast<double>(width_) - 1.0));
        next_p.setY(std::clamp(next_p.y(), 1.0, static_cast<double>(height_) - 1.0));

        if (IsPointInsideAnyPolygon(next_p)) {
            Ray path(center, next_p, 0.0);
            std::optional<QPointF> hit;
            double min_dist_sq = std::numeric_limits<double>::max();

            for (const auto& poly : polygons_) {
                auto intersection = poly.IntersectRay(path);
                if (intersection) {
                    double d = (intersection->x() - center.x()) * (intersection->x() - center.x()) +
                               (intersection->y() - center.y()) * (intersection->y() - center.y());
                    if (d < min_dist_sq) {
                        min_dist_sq = d;
                        hit = intersection;
                    }
                }
            }

            if (hit) {
                double dx = hit->x() - center.x();
                double dy = hit->y() - center.y();
                double len = std::sqrt(dx * dx + dy * dy);
                if (len > 0.5) {
                    double ratio = (len - 0.5) / len;
                    next_p = QPointF(center.x() + dx * ratio, center.y() + dy * ratio);
                } else {
                    next_p = center;
                }
            } else {
                next_p = center;
            }
        }

        multiple_light_sources_.push_back(next_p);
    }
}

std::vector<Polygon> Controller::CreateMultipleLightAreas() const {
    std::vector<Polygon> areas;
    areas.reserve(multiple_light_sources_.size());
    for (const auto& src : multiple_light_sources_) {
        areas.push_back(CreateLightArea(src));
    }
    return areas;
}

void Controller::AddStaticLight(const QPointF& position) {
    if (!IsPointInsideAnyPolygon(position)) {
        static_lights_.push_back(position);
    }
}

void Controller::RemoveLastStaticLight() {
    if (!static_lights_.empty()) {
        static_lights_.pop_back();
    }
}

const std::vector<QPointF>& Controller::GetStaticLights() const {
    return static_lights_;
}

std::vector<Polygon> Controller::CreateStaticLightAreas() const {
    std::vector<Polygon> areas;
    areas.reserve(static_lights_.size());
    for (const auto& src : static_lights_) {
        areas.push_back(CreateLightArea(src));
    }
    return areas;
}

bool Controller::IsPointInsideAnyPolygon(const QPointF& point) const {
    for (size_t i = 1; i < polygons_.size(); ++i) {
        const auto& vertices = polygons_[i].GetVertices();
        bool inside = false;
        for (size_t j = 0, k = vertices.size() - 1; j < vertices.size(); k = j++) {
            if (((vertices[j].y() > point.y()) != (vertices[k].y() > point.y())) &&
                (point.x() < (vertices[k].x() - vertices[j].x()) * (point.y() - vertices[j].y()) /
                                     (vertices[k].y() - vertices[j].y()) +
                                 vertices[j].x())) {
                inside = !inside;
            }
        }
        if (inside) {
            return true;
        }
    }
    return false;
}

bool Controller::IsPolygonValid(const Polygon& polygon) const {
    const auto& v = polygon.GetVertices();
    if (v.size() < 3) {
        return true;
    }

    for (size_t i = 0; i < v.size(); ++i) {
        QPointF p1 = v[i];
        QPointF p2 = v[(i + 1) % v.size()];

        for (size_t j = i + 2; j < v.size(); ++j) {
            if (i == 0 && j == v.size() - 1) {
                continue;
            }
            if (LineSegmentsIntersect(p1, p2, v[j], v[(j + 1) % v.size()])) {
                return false;
            }
        }
    }

    for (const auto& existing : polygons_) {
        const auto& ev = existing.GetVertices();
        for (size_t i = 0; i < v.size(); ++i) {
            QPointF p1 = v[i];
            QPointF p2 = v[(i + 1) % v.size()];
            for (size_t j = 0; j < ev.size(); ++j) {
                if (LineSegmentsIntersect(p1, p2, ev[j], ev[(j + 1) % ev.size()])) {
                    return false;
                }
            }
        }
    }

    return true;
}

void Controller::RemoveLastVertexFromCurrentPolygon() {
    if (!current_polygon_vertices_.empty()) {
        current_polygon_vertices_.pop_back();
    }
}

void Controller::ClearCurrentPolygon() {
    current_polygon_vertices_.clear();
}

void Controller::ClearAllPolygons() {
    polygons_.clear();
    current_polygon_vertices_.clear();
    AddBoundaryPolygon();
}

bool Controller::HasCurrentPolygon() const {
    return !current_polygon_vertices_.empty();
}

const std::vector<QPointF>& Controller::GetCurrentPolygonVertices() const {
    return current_polygon_vertices_;
}

void Controller::UpdateDimensions(int width, int height) {
    width_ = width;
    height_ = height;

    auto clamp = [&](QPointF& p) {
        p.setX(std::clamp(p.x(), 1.0, static_cast<double>(width_) - 1.0));
        p.setY(std::clamp(p.y(), 1.0, static_cast<double>(height_) - 1.0));
    };

    clamp(light_source_);
    for (auto& sl : static_lights_) {
        clamp(sl);
    }

    AddBoundaryPolygon();
    UpdateMultipleLightsCenter(light_source_);
}

int Controller::GetWidth() const {
    return width_;
}

int Controller::GetHeight() const {
    return height_;
}

void Controller::AddBoundaryPolygon() {
    std::vector<QPointF> vertices = {
      QPointF(0.0, 0.0), QPointF(static_cast<double>(width_), 0.0),
      QPointF(static_cast<double>(width_), static_cast<double>(height_)),
      QPointF(0.0, static_cast<double>(height_))};

    if (polygons_.empty()) {
        polygons_.emplace_back(vertices);
    } else {
        polygons_[0] = Polygon(vertices);
    }
}

bool Controller::LineSegmentsIntersect(
    const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4) {
    auto ccw = [](QPointF a, QPointF b, QPointF c) {
        double val = (b.y() - a.y()) * (c.x() - b.x()) - (b.x() - a.x()) * (c.y() - b.y());
        if (std::abs(val) < 1e-9) {
            return 0;
        }
        return (val > 0) ? 1 : 2;
    };

    auto onSegment = [](QPointF p, QPointF a, QPointF b) {
        return p.x() <= std::max(a.x(), b.x()) + 1e-9 && p.x() >= std::min(a.x(), b.x()) - 1e-9 &&
               p.y() <= std::max(a.y(), b.y()) + 1e-9 && p.y() >= std::min(a.y(), b.y()) - 1e-9;
    };

    int o1 = ccw(p1, p2, p3);
    int o2 = ccw(p1, p2, p4);
    int o3 = ccw(p3, p4, p1);
    int o4 = ccw(p3, p4, p2);

    if (o1 != o2 && o3 != o4) {
        return true;
    }

    if (o1 == 0 && onSegment(p3, p1, p2)) {
        return true;
    }
    if (o2 == 0 && onSegment(p4, p1, p2)) {
        return true;
    }
    if (o3 == 0 && onSegment(p1, p3, p4)) {
        return true;
    }
    if (o4 == 0 && onSegment(p2, p3, p4)) {
        return true;
    }

    return false;
}
