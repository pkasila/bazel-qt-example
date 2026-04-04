#include "labs/raycaster/controller.h"

#include <algorithm>
#include <cmath>

namespace {

constexpr double kAngleOffset = 0.0001;
constexpr double kPointEpsilon = 1e-6;
constexpr double kPointEpsilonSquared = kPointEpsilon * kPointEpsilon;
constexpr double kAreaEpsilon = 1.0;
constexpr double kAdjacencyEpsilon = 0.75;
constexpr double kAdjacencyEpsilonSquared = kAdjacencyEpsilon * kAdjacencyEpsilon;
constexpr double kAdjacencyAngleEpsilon = kAngleOffset * 4.0;

double Cross(const QPointF& lhs, const QPointF& rhs) {
    return lhs.x() * rhs.y() - lhs.y() * rhs.x();
}

double SquaredDistance(const QPointF& lhs, const QPointF& rhs) {
    const QPointF delta = lhs - rhs;
    return delta.x() * delta.x() + delta.y() * delta.y();
}

double VectorLength(const QPointF& vector) {
    return std::sqrt(vector.x() * vector.x() + vector.y() * vector.y());
}

double AngleDistance(double lhs, double rhs) {
    constexpr double kTwoPi = 6.28318530717958647692;
    const double delta = std::abs(lhs - rhs);
    return std::min(delta, kTwoPi - delta);
}

double NormalizeAngle(double angle) {
    constexpr double kTwoPi = 6.28318530717958647692;
    while (angle < 0.0) {
        angle += kTwoPi;
    }
    while (angle >= kTwoPi) {
        angle -= kTwoPi;
    }
    return angle;
}

double SignedArea(const std::vector<QPointF>& vertices) {
    if (vertices.size() < 3) {
        return 0.0;
    }

    double doubled_area = 0.0;
    for (std::size_t index = 0; index < vertices.size(); ++index) {
        const QPointF& current = vertices[index];
        const QPointF& next = vertices[(index + 1) % vertices.size()];
        doubled_area += Cross(current, next);
    }
    return 0.5 * doubled_area;
}

std::vector<QPointF> SanitizeVertices(const std::vector<QPointF>& vertices) {
    std::vector<QPointF> sanitized_vertices;
    sanitized_vertices.reserve(vertices.size());

    for (const QPointF& vertex : vertices) {
        if (!sanitized_vertices.empty() &&
            SquaredDistance(sanitized_vertices.back(), vertex) <= kPointEpsilonSquared) {
            continue;
        }
        sanitized_vertices.push_back(vertex);
    }

    if (sanitized_vertices.size() > 1 &&
        SquaredDistance(sanitized_vertices.front(), sanitized_vertices.back()) <=
            kPointEpsilonSquared) {
        sanitized_vertices.pop_back();
    }

    return sanitized_vertices;
}

}  // namespace

Controller::Controller()
    : boundary_polygon_(),
      light_source_(300.0, 250.0),
      scene_top_left_(),
      scene_bottom_right_(),
      has_scene_rect_(false),
      has_active_polygon_(false),
      active_polygon_index_(0) {
}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);

    const std::vector<QPointF>& vertices = polygons_.back().GetVertices();
    has_active_polygon_ =
        vertices.size() == 2 &&
        SquaredDistance(vertices.front(), vertices.back()) < kPointEpsilonSquared;
    if (has_active_polygon_) {
        active_polygon_index_ = polygons_.size() - 1;
    }
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (polygons_.empty()) {
        return;
    }

    const QPointF clamped_vertex = ClampToScene(new_vertex);
    Polygon& polygon = polygons_.back();
    if (has_active_polygon_ && active_polygon_index_ == polygons_.size() - 1) {
        polygon.UpdateLastVertex(clamped_vertex);
        polygon.AddVertex(clamped_vertex);
        return;
    }

    polygon.AddVertex(clamped_vertex);
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!has_active_polygon_ || active_polygon_index_ >= polygons_.size()) {
        return;
    }

    polygons_[active_polygon_index_].UpdateLastVertex(ClampToScene(new_vertex));
}

const QPointF& Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF& light_source) {
    light_source_ = ClampToScene(light_source);
}

std::vector<Ray> Controller::CastRays() const {
    return CastRays(light_source_);
}

std::vector<Ray> Controller::CastRays(const QPointF& source) const {
    std::vector<Ray> rays;
    if (!has_scene_rect_) {
        return rays;
    }

    std::size_t obstacle_vertex_count = boundary_polygon_.GetVertices().size();
    for (std::size_t index = 0; index < polygons_.size(); ++index) {
        if (!IsObstaclePolygon(index)) {
            continue;
        }
        obstacle_vertex_count += polygons_[index].GetVertices().size();
    }
    rays.reserve(obstacle_vertex_count * 3);

    const double cast_radius =
        std::hypot(
            scene_bottom_right_.x() - scene_top_left_.x(),
            scene_bottom_right_.y() - scene_top_left_.y()
        ) * 2.0 + 1.0;

    auto append_rays_for_polygon = [&](const Polygon& polygon) {
        for (const QPointF& vertex : polygon.GetVertices()) {
            const QPointF direction = vertex - source;
            const double length = VectorLength(direction);
            if (length < kPointEpsilon) {
                continue;
            }

            const double angle = NormalizeAngle(std::atan2(direction.y(), direction.x()));
            const QPointF far_end = source + direction / length * cast_radius;
            const Ray long_ray(source, far_end, angle);

            rays.emplace_back(source, vertex, angle);

            Ray clockwise = long_ray.Rotate(-kAngleOffset);
            clockwise.SetAngle(NormalizeAngle(clockwise.GetAngle()));
            rays.push_back(clockwise);

            Ray counter_clockwise = long_ray.Rotate(kAngleOffset);
            counter_clockwise.SetAngle(NormalizeAngle(counter_clockwise.GetAngle()));
            rays.push_back(counter_clockwise);
        }
    };

    append_rays_for_polygon(boundary_polygon_);
    for (std::size_t index = 0; index < polygons_.size(); ++index) {
        if (!IsObstaclePolygon(index)) {
            continue;
        }
        append_rays_for_polygon(polygons_[index]);
    }

    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    if (rays == nullptr) {
        return;
    }

    auto update_ray_with_polygon = [](const Polygon& polygon, Ray* ray) {
        const std::optional<QPointF> intersection = polygon.IntersectRay(*ray);
        if (!intersection.has_value()) {
            return;
        }

        const double current_distance = SquaredDistance(ray->GetBegin(), ray->GetEnd());
        const double candidate_distance = SquaredDistance(ray->GetBegin(), *intersection);
        if (candidate_distance + kPointEpsilonSquared < current_distance) {
            ray->SetEnd(*intersection);
        }
    };

    for (Ray& ray : *rays) {
        update_ray_with_polygon(boundary_polygon_, &ray);
        for (std::size_t index = 0; index < polygons_.size(); ++index) {
            if (!IsObstaclePolygon(index)) {
                continue;
            }
            update_ray_with_polygon(polygons_[index], &ray);
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    if (rays == nullptr || rays->size() < 2) {
        return;
    }

    std::vector<Ray> filtered_rays;
    filtered_rays.reserve(rays->size());
    filtered_rays.push_back(rays->front());

    for (std::size_t index = 1; index < rays->size(); ++index) {
        const Ray& previous_ray = filtered_rays.back();
        const Ray& current_ray = (*rays)[index];
        if (SquaredDistance(previous_ray.GetEnd(), current_ray.GetEnd()) <=
                kAdjacencyEpsilonSquared &&
            AngleDistance(previous_ray.GetAngle(), current_ray.GetAngle()) <=
                kAdjacencyAngleEpsilon) {
            continue;
        }
        filtered_rays.push_back(current_ray);
    }

    if (filtered_rays.size() > 1 &&
        SquaredDistance(filtered_rays.front().GetEnd(), filtered_rays.back().GetEnd()) <=
            kAdjacencyEpsilonSquared &&
        AngleDistance(filtered_rays.front().GetAngle(), filtered_rays.back().GetAngle()) <=
            kAdjacencyAngleEpsilon) {
        filtered_rays.pop_back();
    }

    *rays = std::move(filtered_rays);
}

Polygon Controller::CreateLightArea() const {
    return CreateLightArea(light_source_);
}

Polygon Controller::CreateLightArea(const QPointF& source) const {
    std::vector<Ray> rays = CastRays(source);
    IntersectRays(&rays);
    if (rays.size() < 3) {
        return Polygon();
    }

    std::sort(rays.begin(), rays.end(), [](const Ray& lhs, const Ray& rhs) {
        return lhs.GetAngle() < rhs.GetAngle();
    });

    RemoveAdjacentRays(&rays);

    std::vector<QPointF> vertices;
    vertices.reserve(rays.size());
    for (const Ray& ray : rays) {
        vertices.push_back(ray.GetEnd());
    }

    return Polygon(vertices);
}

std::vector<Polygon> Controller::CreateLightAreas() const {
    const std::vector<QPointF> sources = GetLightSources();
    std::vector<Polygon> light_areas;
    light_areas.reserve(sources.size());

    for (const QPointF& source : sources) {
        light_areas.push_back(CreateLightArea(source));
    }

    return light_areas;
}

void Controller::FinishActivePolygon() {
    if (!has_active_polygon_ || active_polygon_index_ >= polygons_.size()) {
        return;
    }

    const std::vector<QPointF>& current_vertices =
        polygons_[active_polygon_index_].GetVertices();
    std::vector<QPointF> finalized_vertices;
    if (!current_vertices.empty()) {
        finalized_vertices.assign(current_vertices.begin(), current_vertices.end() - 1);
    }
    finalized_vertices = SanitizeVertices(finalized_vertices);

    has_active_polygon_ = false;

    if (finalized_vertices.size() < 3 ||
        std::abs(SignedArea(finalized_vertices)) < kAreaEpsilon) {
        polygons_.erase(polygons_.begin() + static_cast<std::ptrdiff_t>(active_polygon_index_));
        return;
    }

    polygons_[active_polygon_index_] = Polygon(finalized_vertices);
}

bool Controller::HasActivePolygon() const {
    return has_active_polygon_;
}

std::size_t Controller::GetActivePolygonIndex() const {
    return active_polygon_index_;
}

void Controller::SetSceneBounds(const QPointF& top_left, const QPointF& bottom_right) {
    scene_top_left_ = top_left;
    scene_bottom_right_ = bottom_right;
    has_scene_rect_ =
        scene_bottom_right_.x() > scene_top_left_.x() &&
        scene_bottom_right_.y() > scene_top_left_.y();
    if (!has_scene_rect_) {
        boundary_polygon_ = Polygon();
        return;
    }

    boundary_polygon_ = Polygon({
        QPointF(scene_top_left_.x(), scene_top_left_.y()),
        QPointF(scene_bottom_right_.x(), scene_top_left_.y()),
        QPointF(scene_bottom_right_.x(), scene_bottom_right_.y()),
        QPointF(scene_top_left_.x(), scene_bottom_right_.y()),
    });

    const bool inside_x =
        light_source_.x() >= scene_top_left_.x() && light_source_.x() <= scene_bottom_right_.x();
    const bool inside_y =
        light_source_.y() >= scene_top_left_.y() && light_source_.y() <= scene_bottom_right_.y();
    if (!(inside_x && inside_y)) {
        light_source_ = QPointF(
            (scene_top_left_.x() + scene_bottom_right_.x()) * 0.5,
            (scene_top_left_.y() + scene_bottom_right_.y()) * 0.5
        );
    }
    light_source_ = ClampToScene(light_source_);
}

std::vector<QPointF> Controller::GetLightSources() const {
    static const std::vector<QPointF> kOffsets = {
        QPointF(0.0, 0.0),
        QPointF(-4.0, -2.0),
        QPointF(4.0, -2.0),
        QPointF(-3.0, 3.0),
        QPointF(3.0, 3.0),
    };

    std::vector<QPointF> sources;
    sources.reserve(kOffsets.size());
    for (const QPointF& offset : kOffsets) {
        sources.push_back(ClampToScene(light_source_ + offset));
    }
    return sources;
}

const Polygon& Controller::GetBoundaryPolygon() const {
    return boundary_polygon_;
}

QPointF Controller::ClampToScene(const QPointF& point) const {
    if (!has_scene_rect_) {
        return point;
    }

    return QPointF(
        std::clamp(point.x(), scene_top_left_.x(), scene_bottom_right_.x()),
        std::clamp(point.y(), scene_top_left_.y(), scene_bottom_right_.y())
    );
}

bool Controller::IsObstaclePolygon(std::size_t index) const {
    return !(has_active_polygon_ && index == active_polygon_index_);
}
