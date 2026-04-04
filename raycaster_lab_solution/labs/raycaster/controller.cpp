#include "labs/raycaster/controller.h"

#include "labs/raycaster/geometry_utils.h"

#include <QtCore/QSizeF>

#include <algorithm>
#include <cmath>

namespace raycaster {
namespace {

constexpr double kRayAngleOffset = 0.0001;
constexpr double kAdjacentRayDistance = 0.75;
constexpr double kBoundaryMargin = 5.0;

std::vector<QPointF> CreateSoftLightOffsets() {
    return {
        QPointF(0.0, 0.0),
        QPointF(5.0, 0.0),
        QPointF(-5.0, 0.0),
        QPointF(0.0, 5.0),
        QPointF(0.0, -5.0),
    };
}

bool IsUsablePolygon(const Polygon& polygon) {
    return polygon.GetVertices().size() >= 3U;
}

}  // namespace

Controller::Controller()
    : soft_light_offsets_(CreateSoftLightOffsets()) {
}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
    current_polygon_ = polygon;
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (!current_polygon_.has_value()) {
        current_polygon_ = Polygon(std::vector<QPointF>{new_vertex});
        return;
    }

    current_polygon_->AddVertex(new_vertex);
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!current_polygon_.has_value()) {
        return;
    }

    current_polygon_->UpdateLastVertex(new_vertex);
}

const std::optional<Polygon>& Controller::GetCurrentPolygon() const {
    return current_polygon_;
}

void Controller::FinalizeLastPolygon() {
    if (!current_polygon_.has_value()) {
        return;
    }

    const auto& vertices = current_polygon_->GetVertices();
    if (vertices.size() >= 3U) {
        if (!geometry::NearlyEqual(vertices.front(), vertices.back(), 0.5)) {
            polygons_.push_back(*current_polygon_);
        } else {
            std::vector<QPointF> trimmed = vertices;
            trimmed.pop_back();
            if (trimmed.size() >= 3U) {
                polygons_.push_back(Polygon(trimmed));
            }
        }
    }

    current_polygon_.reset();
}

bool Controller::HasCurrentPolygon() const {
    return current_polygon_.has_value();
}

const QPointF& Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF& light_source) {
    light_source_ = light_source;
}

std::vector<QPointF> Controller::GetLightSources() const {
    std::vector<QPointF> sources;
    sources.reserve(soft_light_offsets_.size());
    for (const QPointF& offset : soft_light_offsets_) {
        sources.push_back(light_source_ + offset);
    }
    return sources;
}

void Controller::SetSceneRect(const QRectF& scene_rect) {
    scene_rect_ = scene_rect;
    if (!scene_rect_.isValid()) {
        boundary_polygon_.reset();
        return;
    }

    const QRectF expanded = scene_rect_.adjusted(
        -kBoundaryMargin,
        -kBoundaryMargin,
        kBoundaryMargin,
        kBoundaryMargin);
    boundary_polygon_ = Polygon({
        expanded.topLeft(),
        expanded.topRight(),
        expanded.bottomRight(),
        expanded.bottomLeft(),
    });
}

Polygon Controller::CreateLightArea() const {
    return CreateLightAreaFrom(light_source_);
}

std::vector<Polygon> Controller::CreateSoftLightAreas() const {
    std::vector<Polygon> areas;
    for (const QPointF& source : GetLightSources()) {
        areas.push_back(CreateLightAreaFrom(source));
    }
    return areas;
}

std::vector<Ray> Controller::CastRays() const {
    return CastRaysFrom(light_source_);
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    IntersectRaysFrom(light_source_, rays);
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    RemoveAdjacentRaysFrom(light_source_, rays);
}

std::vector<Polygon> Controller::GetOccluders() const {
    std::vector<Polygon> occluders;
    if (boundary_polygon_.has_value()) {
        occluders.push_back(*boundary_polygon_);
    }

    for (const Polygon& polygon : polygons_) {
        if (IsUsablePolygon(polygon)) {
            occluders.push_back(polygon);
        }
    }

    return occluders;
}

std::vector<Ray> Controller::CastRaysFrom(const QPointF& source) const {
    std::vector<Ray> rays;
    const double max_ray_length = GetMaxRayLength();

    for (const Polygon& polygon : GetOccluders()) {
        for (const QPointF& vertex : polygon.GetVertices()) {
            const double angle = std::atan2(vertex.y() - source.y(), vertex.x() - source.x());
            const QPointF far_point = geometry::PointOnAngle(source, angle, max_ray_length);

            rays.emplace_back(source, far_point, angle);
            rays.push_back(rays.back().Rotate(-kRayAngleOffset));
            rays.push_back(rays.back().Rotate(2.0 * kRayAngleOffset));
        }
    }

    std::sort(rays.begin(), rays.end(), [](const Ray& lhs, const Ray& rhs) {
        return lhs.GetAngle() < rhs.GetAngle();
    });

    return rays;
}

void Controller::IntersectRaysFrom(const QPointF& source, std::vector<Ray>* rays) const {
    if (rays == nullptr) {
        return;
    }

    const auto occluders = GetOccluders();
    for (Ray& ray : *rays) {
        ray.SetBegin(source);

        for (const Polygon& polygon : occluders) {
            const auto intersection = polygon.IntersectRay(ray);
            if (!intersection.has_value()) {
                continue;
            }

            if (geometry::DistanceSquared(source, *intersection) <
                geometry::DistanceSquared(source, ray.GetEnd())) {
                ray.SetEnd(*intersection);
            }
        }
    }
}

void Controller::RemoveAdjacentRaysFrom(const QPointF& source, std::vector<Ray>* rays) const {
    if (rays == nullptr || rays->empty()) {
        return;
    }

    std::sort(rays->begin(), rays->end(), [](const Ray& lhs, const Ray& rhs) {
        return lhs.GetAngle() < rhs.GetAngle();
    });

    std::vector<Ray> filtered;
    filtered.reserve(rays->size());
    filtered.push_back((*rays)[0]);

    for (std::size_t index = 1; index < rays->size(); ++index) {
        if (geometry::Distance(filtered.back().GetEnd(), (*rays)[index].GetEnd()) >
            kAdjacentRayDistance) {
            filtered.push_back((*rays)[index]);
        }
    }

    if (filtered.size() > 1U &&
        geometry::Distance(filtered.front().GetEnd(), filtered.back().GetEnd()) <
            kAdjacentRayDistance) {
        filtered.pop_back();
    }

    for (Ray& ray : filtered) {
        ray.SetBegin(source);
    }

    *rays = std::move(filtered);
}

Polygon Controller::CreateLightAreaFrom(const QPointF& source) const {
    auto rays = CastRaysFrom(source);
    IntersectRaysFrom(source, &rays);
    RemoveAdjacentRaysFrom(source, &rays);

    std::vector<QPointF> vertices;
    vertices.reserve(rays.size());
    for (const Ray& ray : rays) {
        vertices.push_back(ray.GetEnd());
    }

    return Polygon(vertices);
}

double Controller::GetMaxRayLength() const {
    if (!scene_rect_.isValid()) {
        return 5000.0;
    }

    const QSizeF size = scene_rect_.size();
    return std::hypot(size.width(), size.height()) * 2.0 + 50.0;
}

}  // namespace raycaster
