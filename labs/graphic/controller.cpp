#include "controller.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr double kRayAngleOffset = 0.0001;
constexpr double kPointMergeDistance = 1.0;
constexpr double kAngleMergeDistance = 0.00001;

double Distance(const QPointF& a, const QPointF& b) {
    return std::hypot(a.x() - b.x(), a.y() - b.y());
}

}

Controller::Controller()
    : light_source_(300.0, 300.0),
      scene_size_(800.0, 600.0),
      boundary_(std::vector<QPointF>{QPointF(0.0, 0.0), QPointF(800.0, 0.0), QPointF(800.0, 600.0), QPointF(0.0, 600.0)}),
      dynamic_light_offsets_{
          QPointF(0.0, 0.0),
          QPointF(-6.0, 0.0),
          QPointF(6.0, 0.0),
          QPointF(0.0, -6.0),
          QPointF(0.0, 6.0),
          QPointF(-4.0, -4.0),
          QPointF(4.0, 4.0),
      },
      has_active_polygon_(false) {}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
    has_active_polygon_ = true;
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

void Controller::FinalizeLastPolygon() {
    if (polygons_.empty()) {
        has_active_polygon_ = false;
        return;
    }

    polygons_.back().RemoveLastVertex();
    const std::vector<QPointF> vertices = polygons_.back().GetVertices();

    if (vertices.size() < 3) {
        polygons_.pop_back();
    }

    has_active_polygon_ = false;
}

const QPointF& Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF& light_source) {
    const double x = std::clamp(light_source.x(), 0.0, scene_size_.width());
    const double y = std::clamp(light_source.y(), 0.0, scene_size_.height());
    light_source_ = QPointF(x, y);
}

void Controller::SetSceneSize(const QSizeF& size) {
    scene_size_ = size;
    boundary_ = Polygon(std::vector<QPointF>{
        QPointF(0.0, 0.0),
        QPointF(scene_size_.width(), 0.0),
        QPointF(scene_size_.width(), scene_size_.height()),
        QPointF(0.0, scene_size_.height()),
    });
    SetLightSource(light_source_);
}

bool Controller::HasActivePolygon() const {
    return has_active_polygon_;
}

std::vector<QPointF> Controller::GetLightSources() const {
    std::vector<QPointF> sources;
    sources.reserve(dynamic_light_offsets_.size());
    for (const QPointF& offset : dynamic_light_offsets_) {
        QPointF point = light_source_ + offset;
        point.setX(std::clamp(point.x(), 0.0, scene_size_.width()));
        point.setY(std::clamp(point.y(), 0.0, scene_size_.height()));
        sources.push_back(point);
    }
    return sources;
}

std::vector<Ray> Controller::CastRays() const {
    return CastRaysForSource(light_source_);
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    IntersectRaysForSource(light_source_, rays);
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
        const Ray& current = (*rays)[i];
        const Ray& previous = filtered.back();
        const double angle_diff = std::abs(current.GetAngle() - previous.GetAngle());
        if (Distance(current.GetEnd(), previous.GetEnd()) > kPointMergeDistance || angle_diff > kAngleMergeDistance) {
            filtered.push_back(current);
        }
    }

    if (filtered.size() > 1 && Distance(filtered.front().GetEnd(), filtered.back().GetEnd()) <= kPointMergeDistance) {
        filtered.pop_back();
    }

    *rays = std::move(filtered);
}

Polygon Controller::CreateLightArea() const {
    return CreateLightAreaForSource(light_source_);
}

Polygon Controller::CreateLightAreaForSource(const QPointF& source) const {
    std::vector<Ray> rays = CastRaysForSource(source);
    std::sort(rays.begin(), rays.end(), [](const Ray& lhs, const Ray& rhs) {
        return lhs.GetAngle() < rhs.GetAngle();
    });
    IntersectRaysForSource(source, &rays);
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

std::vector<Polygon> Controller::AllPolygons() const {
    std::vector<Polygon> result;
    result.reserve(polygons_.size() + 1);

    for (std::size_t i = 0; i < polygons_.size(); ++i) {
        if (has_active_polygon_ && i + 1 == polygons_.size()) {
            continue;
        }
        result.push_back(polygons_[i]);
    }

    result.push_back(boundary_);
    return result;
}

std::vector<Ray> Controller::CastRaysForSource(const QPointF& source) const {
    std::vector<Ray> rays;
    const std::vector<Polygon> polygons = AllPolygons();

    for (const Polygon& polygon : polygons) {
        for (const QPointF& vertex : polygon.GetVertices()) {
            const QPointF direction = vertex - source;
            if (Distance(source, vertex) < 1e-9) {
                continue;
            }
            const double angle = std::atan2(direction.y(), direction.x());
            Ray ray(source, vertex, angle);
            rays.push_back(ray);
            rays.push_back(ray.Rotate(-kRayAngleOffset));
            rays.push_back(ray.Rotate(kRayAngleOffset));
        }
    }

    return rays;
}

void Controller::IntersectRaysForSource(const QPointF& source, std::vector<Ray>* rays) const {
    if (rays == nullptr) {
        return;
    }

    const std::vector<Polygon> polygons = AllPolygons();
    const double range = SceneRange();

    for (Ray& ray : *rays) {
        const QPointF far_end(
            source.x() + std::cos(ray.GetAngle()) * range,
            source.y() + std::sin(ray.GetAngle()) * range
        );
        ray.SetBegin(source);
        ray.SetEnd(far_end);

        double best_distance = Distance(ray.GetBegin(), ray.GetEnd());
        QPointF best_point = ray.GetEnd();

        for (const Polygon& polygon : polygons) {
            const auto intersection = polygon.IntersectRay(ray);
            if (!intersection.has_value()) {
                continue;
            }
            const double distance = Distance(ray.GetBegin(), *intersection);
            if (distance < best_distance) {
                best_distance = distance;
                best_point = *intersection;
            }
        }

        ray.SetEnd(best_point);
    }
}

double Controller::SceneRange() const {
    return std::hypot(scene_size_.width(), scene_size_.height()) * 2.0 + 10.0;
}
