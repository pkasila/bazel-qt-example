#include "labs/raycaster/core/controller.h"

#include "labs/raycaster/core/geometry_utils.h"
#include "labs/raycaster/core/ray.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

namespace raycaster {

Controller::Controller()
    : light_source_(200.0, 200.0)
    , scene_left_(0.0)
    , scene_top_(0.0)
    , scene_right_(800.0)
    , scene_bottom_(600.0)
    , boundary_polygon_(
          {QPointF(0.0, 0.0), QPointF(800.0, 0.0), QPointF(800.0, 600.0), QPointF(0.0, 600.0)})
    , is_drawing_polygon_(false)
    , soft_shadows_enabled_(true)
    , dynamic_light_offsets_({
        QPointF(0.0, 0.0),
        QPointF(-6.0, -4.0),
        QPointF(6.0, -4.0),
        QPointF(-6.0, 4.0),
        QPointF(6.0, 4.0),
      }) {
}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
    is_drawing_polygon_ = true;
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

const QPointF& Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF& light_source) {
    if (CanMoveDynamicLightsTo(light_source)) {
        light_source_ = light_source;
    }
}

bool Controller::AreSoftShadowsEnabled() const {
    return soft_shadows_enabled_;
}

void Controller::SetSoftShadowsEnabled(bool enabled) {
    if (soft_shadows_enabled_ == enabled) {
        return;
    }

    soft_shadows_enabled_ = enabled;
    if (soft_shadows_enabled_) {
        const QPointF clamped = ClampToScene(light_source_, 1.0);
        if (!CanMoveDynamicLightsTo(light_source_)) {
            RelocateDynamicLightsNear(clamped);
        }
    }
}

void Controller::SetSceneBounds(double left, double top, double right, double bottom) {
    scene_left_ = std::min(left, right);
    scene_top_ = std::min(top, bottom);
    scene_right_ = std::max(left, right);
    scene_bottom_ = std::max(top, bottom);

    boundary_polygon_ = Polygon({
      QPointF(scene_left_, scene_top_),
      QPointF(scene_right_, scene_top_),
      QPointF(scene_right_, scene_bottom_),
      QPointF(scene_left_, scene_bottom_),
    });

    static_lights_.erase(
        std::remove_if(
            static_lights_.begin(), static_lights_.end(),
            [this](const QPointF& source) {
                return !IsInsideScene(source) || IsPointInsideAnyObstacle(source);
            }),
        static_lights_.end());

    const QPointF clamped = ClampToScene(light_source_, 1.0);
    if (!CanMoveDynamicLightsTo(light_source_) && !RelocateDynamicLightsNear(clamped)) {
        light_source_ = clamped;
    }
}

bool Controller::IsDrawingPolygon() const {
    return is_drawing_polygon_;
}

bool Controller::FinalizeLastPolygon() {
    if (!is_drawing_polygon_ || polygons_.empty()) {
        return false;
    }

    auto sanitized = SanitizePolygonVertices(polygons_.back().GetVertices());
    if (sanitized.size() < 3U || !geom::HasSignificantArea(sanitized)) {
        polygons_.pop_back();
        is_drawing_polygon_ = false;
        return false;
    }

    if (!geom::IsSimplePolygon(sanitized)) {
        polygons_.pop_back();
        is_drawing_polygon_ = false;
        return false;
    }

    for (std::size_t i = 0; i + 1U < polygons_.size(); ++i) {
        const auto existing = SanitizePolygonVertices(polygons_[i].GetVertices());
        if (existing.size() < 3U) {
            continue;
        }
        if (geom::PolygonsIntersect(sanitized, existing)) {
            polygons_.pop_back();
            is_drawing_polygon_ = false;
            return false;
        }
    }

    for (const QPointF& source : GetDynamicLightSources()) {
        if (geom::PointInPolygon(source, sanitized)) {
            polygons_.pop_back();
            is_drawing_polygon_ = false;
            return false;
        }
    }

    for (const QPointF& source : static_lights_) {
        if (geom::PointInPolygon(source, sanitized)) {
            polygons_.pop_back();
            is_drawing_polygon_ = false;
            return false;
        }
    }

    polygons_.back() = Polygon(sanitized);
    is_drawing_polygon_ = false;
    return true;
}

void Controller::Clear() {
    polygons_.clear();
    static_lights_.clear();
    is_drawing_polygon_ = false;
}

std::vector<Ray> Controller::CastRays() const {
    return CastRaysFrom(light_source_);
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    if (rays == nullptr) {
        return;
    }

    const auto obstacles = GetObstaclePolygons();
    for (Ray& ray : *rays) {
        for (const Polygon* polygon : obstacles) {
            const auto intersection = polygon->IntersectRay(ray);
            if (!intersection.has_value()) {
                continue;
            }

            const double current_distance = geom::DistanceSquared(ray.GetBegin(), ray.GetEnd());
            const double intersection_distance =
                geom::DistanceSquared(ray.GetBegin(), intersection.value());

            if (intersection_distance + 1e-7 < current_distance) {
                ray.SetEnd(intersection.value());
            }
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    if (rays == nullptr || rays->empty()) {
        return;
    }

    std::sort(rays->begin(), rays->end(), [](const Ray& lhs, const Ray& rhs) {
        return lhs.GetAngle() < rhs.GetAngle();
    });

    std::vector<Ray> filtered;
    filtered.reserve(rays->size());
    filtered.push_back(rays->front());

    for (std::size_t i = 1; i < rays->size(); ++i) {
        if (geom::Distance(filtered.back().GetEnd(), (*rays)[i].GetEnd()) >
            geom::kAdjacentPointDistance) {
            filtered.push_back((*rays)[i]);
        }
    }

    if (filtered.size() > 1U &&
        geom::Distance(filtered.front().GetEnd(), filtered.back().GetEnd()) <=
            geom::kAdjacentPointDistance) {
        filtered.pop_back();
    }

    *rays = std::move(filtered);
}

Polygon Controller::CreateLightArea() const {
    return CreateLightAreaForSource(light_source_);
}

std::vector<QPointF> Controller::GetDynamicLightSources() const {
    std::vector<QPointF> sources;
    if (!soft_shadows_enabled_) {
        if (IsInsideScene(light_source_) && !IsPointInsideAnyObstacle(light_source_)) {
            sources.push_back(light_source_);
        }
        return sources;
    }

    sources.reserve(dynamic_light_offsets_.size());
    for (const QPointF& offset : dynamic_light_offsets_) {
        const QPointF candidate = geom::Add(light_source_, offset);
        if (IsInsideScene(candidate) && !IsPointInsideAnyObstacle(candidate)) {
            sources.push_back(candidate);
        }
    }

    return sources;
}

const std::vector<QPointF>& Controller::GetStaticLights() const {
    return static_lights_;
}

void Controller::AddStaticLight(const QPointF& light_source) {
    if (IsInsideScene(light_source) && !IsPointInsideAnyObstacle(light_source)) {
        static_lights_.push_back(light_source);
    }
}

void Controller::ClearStaticLights() {
    static_lights_.clear();
}

Polygon Controller::CreateLightAreaForSource(const QPointF& light_source) const {
    auto rays = CastRaysFrom(light_source);
    IntersectRays(&rays);

    std::sort(rays.begin(), rays.end(), [](const Ray& lhs, const Ray& rhs) {
        return lhs.GetAngle() < rhs.GetAngle();
    });

    RemoveAdjacentRays(&rays);

    std::sort(rays.begin(), rays.end(), [](const Ray& lhs, const Ray& rhs) {
        return lhs.GetAngle() < rhs.GetAngle();
    });

    std::vector<QPointF> vertices;
    vertices.reserve(rays.size());
    for (const Ray& ray : rays) {
        vertices.push_back(ray.GetEnd());
    }

    return Polygon(vertices);
}

bool Controller::IsPointInsideAnyObstacle(const QPointF& point) const {
    for (const Polygon* polygon : GetObstaclePolygons()) {
        if (polygon == &boundary_polygon_) {
            continue;
        }
        const auto vertices = SanitizePolygonVertices(polygon->GetVertices());
        if (vertices.size() >= 3U && geom::PointInPolygon(point, vertices)) {
            return true;
        }
    }
    return false;
}

std::vector<const Polygon*> Controller::GetObstaclePolygons() const {
    std::vector<const Polygon*> polygons;
    polygons.reserve(polygons_.size() + 1U);

    for (std::size_t i = 0; i < polygons_.size(); ++i) {
        if (is_drawing_polygon_ && i + 1U == polygons_.size()) {
            continue;
        }
        if (SanitizePolygonVertices(polygons_[i].GetVertices()).size() >= 3U) {
            polygons.push_back(&polygons_[i]);
        }
    }

    polygons.push_back(&boundary_polygon_);
    return polygons;
}

std::vector<Ray> Controller::CastRaysFrom(const QPointF& light_source) const {
    std::vector<Ray> rays;
    const auto obstacles = GetObstaclePolygons();

    for (const Polygon* polygon : obstacles) {
        for (const QPointF& vertex : polygon->GetVertices()) {
            const QPointF direction = geom::Subtract(vertex, light_source);
            if (geom::LengthSquared(direction) <= 1e-9) {
                continue;
            }

            const double angle = geom::NormalizeAngle(std::atan2(direction.y(), direction.x()));

            Ray central(light_source, vertex, angle);
            rays.push_back(central);
            rays.push_back(central.Rotate(-geom::kRotationEpsilon));
            rays.push_back(central.Rotate(geom::kRotationEpsilon));
        }
    }

    return rays;
}

bool Controller::CanMoveDynamicLightsTo(const QPointF& center) const {
    if (!IsInsideScene(center)) {
        return false;
    }

    if (!soft_shadows_enabled_) {
        return !IsPointInsideAnyObstacle(center);
    }

    for (const QPointF& offset : dynamic_light_offsets_) {
        const QPointF candidate = geom::Add(center, offset);
        if (!IsInsideScene(candidate) || IsPointInsideAnyObstacle(candidate)) {
            return false;
        }
    }

    return true;
}

bool Controller::IsInsideScene(const QPointF& point, double padding) const {
    return point.x() >= scene_left_ + padding && point.x() <= scene_right_ - padding &&
           point.y() >= scene_top_ + padding && point.y() <= scene_bottom_ - padding;
}

QPointF Controller::ClampToScene(const QPointF& point, double padding) const {
    const double min_x = std::min(scene_left_ + padding, scene_right_ - padding);
    const double max_x = std::max(scene_left_ + padding, scene_right_ - padding);
    const double min_y = std::min(scene_top_ + padding, scene_bottom_ - padding);
    const double max_y = std::max(scene_top_ + padding, scene_bottom_ - padding);
    return {std::clamp(point.x(), min_x, max_x), std::clamp(point.y(), min_y, max_y)};
}

bool Controller::RelocateDynamicLightsNear(const QPointF& preferred_center) {
    if (CanMoveDynamicLightsTo(preferred_center)) {
        light_source_ = preferred_center;
        return true;
    }

    const QPointF scene_center(
        0.5 * (scene_left_ + scene_right_), 0.5 * (scene_top_ + scene_bottom_));
    if (CanMoveDynamicLightsTo(scene_center)) {
        light_source_ = scene_center;
        return true;
    }

    const double padding = 8.0;
    const double step = 18.0;
    bool found = false;
    QPointF best_candidate = preferred_center;
    double best_distance = std::numeric_limits<double>::infinity();

    for (double y = scene_top_ + padding; y <= scene_bottom_ - padding; y += step) {
        for (double x = scene_left_ + padding; x <= scene_right_ - padding; x += step) {
            const QPointF candidate(x, y);
            if (!CanMoveDynamicLightsTo(candidate)) {
                continue;
            }

            const double distance = geom::DistanceSquared(candidate, preferred_center);
            if (!found || distance < best_distance) {
                found = true;
                best_candidate = candidate;
                best_distance = distance;
            }
        }
    }

    if (found) {
        light_source_ = best_candidate;
        return true;
    }

    return false;
}

std::vector<QPointF> Controller::SanitizePolygonVertices(const std::vector<QPointF>& vertices) {
    std::vector<QPointF> sanitized;
    sanitized.reserve(vertices.size());

    for (const QPointF& vertex : vertices) {
        if (sanitized.empty() || !geom::AlmostSamePoint(sanitized.back(), vertex)) {
            sanitized.push_back(vertex);
        }
    }

    if (sanitized.size() >= 2U && geom::AlmostSamePoint(sanitized.front(), sanitized.back())) {
        sanitized.pop_back();
    }

    return sanitized;
}

}  // namespace raycaster
