#include "../input/controller.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <set>
#include <numbers>

Controller::Controller() {
    regenerateLightSources();
}

bool Controller::isBoundary(const Polygon& poly) const {
    return !polygons_.empty() && &poly == &polygons_[0];
}
bool Controller::isBoundaryIndex(size_t index) const {
    return index == 0 && !polygons_.empty();
}

bool Controller::isPointInsideAnyPolygon(const QPointF& point, bool includeBoundary) const {
    for (size_t i = 0; i < polygons_.size(); ++i) {
        if (!includeBoundary && isBoundaryIndex(i)) {
            continue;
        }
        if (polygons_[i].containsPoint(point)) {
            return true;
        }
    }
    return false;
}

bool Controller::isLightSourceInside(const Polygon& poly) const {
    for(const auto& light_pos : light_sources_) {
        if (poly.containsPoint(light_pos)) {
             return true;
        }
    }
    for(const auto& light_pos : static_light_sources_) {
        if (poly.containsPoint(light_pos)) {
             return true;
        }
    }
    return false;
}

bool Controller::addPolygon(Polygon&& polygon) {
    if (polygon.vertexCount() < 3) {
         current_polygon_ = std::nullopt;
         return false;
    }
    for (const auto& existing_poly : polygons_) {
        if (isBoundary(existing_poly)) {
            continue;
        }
        if (polygon.intersects(existing_poly)) {
            current_polygon_ = std::nullopt;
            return false;
        }
    }
    if (isLightSourceInside(polygon)) {
        current_polygon_ = std::nullopt;
        return false;
    }

    polygons_.push_back(std::move(polygon));
    current_polygon_ = std::nullopt;
    return true;
}

void Controller::startNewPolygon(const QPointF& first_vertex) {
    finalizeCurrentPolygon();
    current_polygon_ = Polygon({first_vertex, first_vertex});
}

void Controller::addVertexToCurrentPolygon(const QPointF& vertex) {
    if (current_polygon_) {
        current_polygon_->updateLastVertex(vertex);
        current_polygon_->addVertex(vertex);
    }
}

void Controller::updateCurrentPolygonLastVertex(const QPointF& new_vertex) {
     if (current_polygon_) {
        current_polygon_->updateLastVertex(new_vertex);
    }
}

bool Controller::finalizeCurrentPolygon() {
    bool added = false;
    if (current_polygon_) {
        if (current_polygon_->vertexCount() > 2) {
            auto vertices = current_polygon_->getVertices();
            if (!vertices.empty()) {
                vertices.pop_back();
                if (vertices.size() >= 3) {
                    added = addPolygon(Polygon(vertices));
                }
            }
        }
    }
    if (!added) {
        current_polygon_ = std::nullopt;
    }
    return added;
}

void Controller::addBoundaryPolygon(qreal minX, qreal minY, qreal maxX, qreal maxY) {
    constexpr qreal margin = 1.0;
    std::vector<QPointF> boundary_vertices = {
        QPointF(minX - margin, minY - margin),
        QPointF(maxX + margin, minY - margin),
        QPointF(maxX + margin, maxY + margin),
        QPointF(minX - margin, maxY + margin)
    };
    if (polygons_.empty()) {
         polygons_.insert(polygons_.begin(), Polygon(std::move(boundary_vertices)));
    } else {
        polygons_[0] = Polygon(std::move(boundary_vertices));
    }
}

bool Controller::setLightSourceCenter(const QPointF& center) {
    if (isPointInsideAnyPolygon(center)) {
         return false;
    }
    if (light_source_center_ != center) {
        light_source_center_ = center;
        regenerateLightSources();
    }
    return true;
}

bool Controller::addStaticLightSource(const QPointF& pos) {
    if (isPointInsideAnyPolygon(pos)) {
        return false;
    }
    static_light_sources_.push_back(pos);
    return true;
}

bool Controller::removePolygon(size_t index) {
    const size_t actual_index = index + 1;
    if (actual_index < polygons_.size()) {
        polygons_.erase(polygons_.cbegin() + static_cast<long>(actual_index));
        return true;
    }
    return false;
}

bool Controller::removeStaticLightSource(size_t index) {
    if (index < static_light_sources_.size()) {
        static_light_sources_.erase(static_light_sources_.cbegin() + static_cast<long>(index));
        return true;
    }
    return false;
}


void Controller::regenerateLightSources() {
    light_sources_.clear();
    if (num_light_sources_ <= 0) {
        return;
    }
    light_sources_.push_back(light_source_center_);
    const int sources_to_add = num_light_sources_ - 1;
    if (sources_to_add > 0 && light_source_spread_ > 1e-6) {
        const double angle_step = 2.0 * std::numbers::pi / sources_to_add;
        for (int i = 0; i < sources_to_add; ++i) {
            const double angle = static_cast<double>(i) * angle_step;
            const QPointF offset(light_source_spread_ * std::cos(angle),
                           light_source_spread_ * std::sin(angle));
            light_sources_.push_back(light_source_center_ + offset);
        }
    }
}

std::optional<QPointF> Controller::intersectSegments(const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4) const
{
    const double x1 = p1.x();
    const double y1 = p1.y();
    const double x2 = p2.x();
    const double y2 = p2.y();
    const double x3 = p3.x();
    const double y3 = p3.y();
    const double x4 = p4.x();
    const double y4 = p4.y();
    const double den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    constexpr double epsilon = 1e-9;

    if (std::abs(den) < epsilon) {
         return std::nullopt;
    }
    const double t_num = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
    const double u_num = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3));
    const double t = t_num / den;
    const double u = u_num / den;

    if (t >= -epsilon && t <= 1.0 + epsilon && u >= -epsilon && u <= 1.0 + epsilon) {
        QPointF intersection_point;
        intersection_point.setX(x1 + t * (x2 - x1));
        intersection_point.setY(y1 + t * (y2 - y1));
        return intersection_point;
    }
    return std::nullopt;
}

std::set<QPointF, QPointFCompare> Controller::findAllSelfIntersectionPoints() const {
    std::set<QPointF, QPointFCompare> intersection_points;
    for (const auto& poly : polygons_) {
        if (isBoundary(poly)) {
            continue;
        }
        const auto& vertices = poly.getVertices();
        const size_t n = vertices.size();
        if (n < 4) {
             continue;
        }
        for (size_t i = 0; i < n; ++i) {
            const QPointF& p1 = vertices[i];
            const QPointF& p2 = vertices[(i + 1) % n];
            for (size_t j = i + 2; j < (i == 0 ? n - 1 : n) ; ++j) {
                const QPointF& p3 = vertices[j];
                const QPointF& p4 = vertices[(j + 1) % n];
                const std::optional<QPointF> intersection = intersectSegments(p1, p2, p3, p4);
                if (intersection) {
                    intersection_points.insert(*intersection);
                }
            }
        }
    }
    return intersection_points;
}

std::vector<Ray> Controller::castRays(const QPointF& source) const {
    std::vector<Ray> rays;
    const std::set<QPointF, QPointFCompare> self_intersection_points = findAllSelfIntersectionPoints();
    size_t total_vertices = 0;
    for (const auto& poly : polygons_) {
        total_vertices += poly.vertexCount();
    }
    rays.reserve(total_vertices * 3 + self_intersection_points.size() * 3 + 12);

    for (const auto& poly : polygons_) {
        for (const auto& vertex : poly.getVertices()) {
            const QPointF delta = vertex - source;
            const double angle = std::atan2(delta.y(), delta.x());
            const double dist_sq = delta.x()*delta.x() + delta.y()*delta.y();
            if (dist_sq > 1e-9) {
                 rays.emplace_back(source, angle, MAX_RAY_LENGTH);
            }
            rays.emplace_back(source, angle - RAY_ANGLE_OFFSET, MAX_RAY_LENGTH);
            rays.emplace_back(source, angle + RAY_ANGLE_OFFSET, MAX_RAY_LENGTH);
        }
    }

    for (const QPointF& point : self_intersection_points) {
         const QPointF delta = point - source;
         const double angle = std::atan2(delta.y(), delta.x());
         const double dist_sq = delta.x()*delta.x() + delta.y()*delta.y();
         if (dist_sq > 1e-9) {
            rays.emplace_back(source, angle, MAX_RAY_LENGTH);
         }
         rays.emplace_back(source, angle - RAY_ANGLE_OFFSET, MAX_RAY_LENGTH);
         rays.emplace_back(source, angle + RAY_ANGLE_OFFSET, MAX_RAY_LENGTH);
    }
    return rays;
}

void Controller::intersectRays(std::vector<Ray>* rays) const {
    if (!rays) {
         return;
    }
    constexpr double origin_epsilon_sq = 1e-9;
    for (Ray& ray : *rays) {
        const QPointF ray_start = ray.getBegin();
        QPointF closest_hit = ray.getEnd();
        double min_dist_sq = distSq(ray_start, closest_hit);
        for (const auto& poly : polygons_) {
            const std::optional<QPointF> intersection = poly.intersectRay(ray);
            if (intersection) {
                const double dist_sq = distSq(ray_start, *intersection);
                if (dist_sq < origin_epsilon_sq) {
                    continue;
                }
                if (dist_sq < min_dist_sq) {
                    min_dist_sq = dist_sq;
                    closest_hit = *intersection;
                }
            }
        }
        ray.setEnd(closest_hit);
    }
}

void Controller::removeAdjacentRays(std::vector<Ray>* rays) const {
    if (!rays || rays->size() < 2) {
        return;
    }
    std::sort(rays->begin(), rays->end(), [](const Ray& a, const Ray& b) {
        return a.getAngle() < b.getAngle();
    });
    auto it = std::unique(rays->begin(), rays->end(),
                          [this](const Ray& a, const Ray& b) {
                              return distSq(a.getEnd(), b.getEnd()) < ADJACENT_RAY_DIST_THRESHOLD_SQ;
                          });
    rays->erase(it, rays->end());
}

std::vector<Polygon> Controller::createLightAreasForSources(const std::vector<QPointF>& sources) const {
    std::vector<Polygon> light_areas;
    light_areas.reserve(sources.size());
    for (const auto& source : sources) {
        std::vector<Ray> rays = castRays(source);
        if (rays.empty()) {
            continue;
        }
        intersectRays(&rays);
        removeAdjacentRays(&rays);
        if (rays.size() >= 3) {
            std::vector<QPointF> light_vertices;
            light_vertices.reserve(rays.size());
            for (const auto& ray : rays) {
                light_vertices.push_back(ray.getEnd());
            }
            light_areas.emplace_back(std::move(light_vertices));
        }
    }
    return light_areas;
}

std::vector<Polygon> Controller::createDynamicLightAreas() const {
    return createLightAreasForSources(light_sources_);
}

std::vector<Polygon> Controller::createStaticLightAreas() const {
    return createLightAreasForSources(static_light_sources_);
}

size_t Controller::getPolygonCount() const {
    return polygons_.empty() ? 0 : polygons_.size() - 1;
}