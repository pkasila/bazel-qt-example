#include "Controller.h"

#include "geometry/GeometryUtils.h"

#include <algorithm>
#include <cmath>

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (polygons_.empty()) {
        return;
    }
    polygons_.back().AddVertex(new_vertex);
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (polygons_.empty()) {
        return;
    }
    polygons_.back().UpdateLastVertex(new_vertex);
}

const QPointF& Controller::GetLightSource() const {
    return light_source_;
}

void Controller::SetLightSource(const QPointF& light_source) {
    light_source_ = light_source;
}

const std::vector<QPointF>& Controller::GetLightOffsets() const {
    return light_offsets_;
}

void Controller::SetLightOffsets(const std::vector<QPointF>& offsets) {
    light_offsets_ = offsets;
}

std::vector<QPointF> Controller::GetAllLightSources() const {
    std::vector<QPointF> result;
    result.reserve(light_offsets_.size());

    for (const auto& offset : light_offsets_) {
        result.push_back(light_source_ + offset);
    }

    return result;
}


std::vector<Ray> Controller::CastRays(const QPointF& light_source) const {
    const double del_angle = 0.0001;
    const double ray_length = 10000.0;

    std::vector<Ray> res;

    for (const auto& polygon : polygons_) {
        for (const auto& vertex : polygon.GetVertices()) {
            QPointF dir = vertex - light_source;

            double len2 = geometry::LengthSquared(dir);
            if (len2 < 1e-12) {
                continue;
            }

            double len = std::sqrt(len2);
            QPointF normalized = dir / len;
            QPointF far_end = light_source + normalized * ray_length;

            double angle = geometry::AngleOf(dir);
            Ray ray(light_source, far_end, angle);

            res.push_back(ray);
            res.push_back(ray.Rotate(del_angle));
            res.push_back(ray.Rotate(-del_angle));
        }
    }

    return res;
}


void Controller::IntersectRays(std::vector<Ray>* rays) const {
    for (auto& ray : *rays) {
        for (const auto& polygon : polygons_) {
            std::optional<QPointF> intersect = polygon.IntersectRay(ray);
            if (intersect == std::nullopt) {
                continue;
            }

            if (geometry::DistanceSquared(ray.GetBegin(), *intersect) <
                geometry::DistanceSquared(ray.GetBegin(), ray.GetEnd())) {
                ray.SetEnd(*intersect);
            }
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    if (rays == nullptr || rays->size() <= 1) {
        return;
    }

    const double eps = 1e-8;
    std::vector<Ray> filtered;
    filtered.push_back((*rays)[0]);

    for (size_t i = 1; i < rays->size(); ++i) {
        const QPointF& current_end = (*rays)[i].GetEnd();
        const QPointF& last_end = filtered.back().GetEnd();

        if (geometry::DistanceSquared(current_end, last_end) > eps) {
            filtered.push_back((*rays)[i]);
        }
    }

    if (filtered.size() > 1) {
        const QPointF& first_end = filtered.front().GetEnd();
        const QPointF& last_end = filtered.back().GetEnd();

        if (geometry::DistanceSquared(first_end, last_end) <= eps) {
            filtered.pop_back();
        }
    }

    *rays = std::move(filtered);
}

Polygon Controller::CreateLightArea(const QPointF& light_source) const {
    std::vector<Ray> rays = CastRays(light_source);

    IntersectRays(&rays);

    std::ranges::sort(rays, [](const Ray& a, const Ray& b) {
        return a.GetAngle() < b.GetAngle();
    });

    RemoveAdjacentRays(&rays);

    std::vector<QPointF> vertices;
    for (const auto& ray : rays) {
        vertices.push_back(ray.GetEnd());
    }

    return Polygon(vertices);
}

Polygon& Controller::GetLastPolygon() {
    return polygons_.back();
}

const Polygon& Controller::GetLastPolygon() const {
    return polygons_.back();
}

void Controller::RemoveLastVertexFromLastPolygon() {
    if (polygons_.empty()) {
        return;
    }
    polygons_.back().RemoveLastVertex();
}

void Controller::RemoveLastPolygon() {
    if (!polygons_.empty()) {
        polygons_.pop_back();
    }
}

void Controller::SetPolygon(size_t index, const Polygon& polygon) {
    if (index < polygons_.size()) {
        polygons_[index] = polygon;
    }
}

size_t Controller::PolygonCount() const {
    return polygons_.size();
}

bool Controller::Empty() const {
    return polygons_.empty();
}

bool Controller::IsLightSourcePositionValid(const QPointF& pos) const {
    for (const auto& offset : light_offsets_) {
        const QPointF light_point = pos + offset;

        for (size_t i = 0; i < polygons_.size(); ++i) {
            if (i == 0) {
                continue;
            }

            if (polygons_[i].ContainsPoint(light_point) || polygons_[i].OnBoundary(light_point)) {
                return false;
            }
        }
    }

    return true;
}

bool Controller::CanPlacePolygon(const Polygon& polygon, int ignore_index, bool treat_as_open) const {
    if (polygon.SelfIntersects(!treat_as_open)) {
        return false;
    }

    if (polygon.VertexCount() < 2) {
        return true;
    }

    for (size_t i = 0; i < polygons_.size(); ++i) {
        if (i == 0) {
            continue;
        }
        if (static_cast<int>(i) == ignore_index) {
            continue;
        }

        if (polygon.IntersectsPolygon(polygons_[i], !treat_as_open)) {
            return false;
        }
    }

    return true;
}

const std::vector<QPointF>& Controller::GetStaticLightSources() const {
    return static_light_sources_;
}

void Controller::AddStaticLightSource(const QPointF& light_source) {
    static_light_sources_.push_back(light_source);
}

void Controller::ClearStaticLightSources() {
    static_light_sources_.clear();
}

std::vector<QPointF> Controller::GetAllStaticLightPoints() const {
    std::vector<QPointF> result;
    result.reserve(static_light_sources_.size() * light_offsets_.size());

    for (const auto& source_center : static_light_sources_) {
        for (const auto& offset : light_offsets_) {
            result.push_back(source_center + offset);
        }
    }

    return result;
}
