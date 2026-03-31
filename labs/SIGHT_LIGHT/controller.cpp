#include "controller.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <QDebug>


double Controller::pointDistanceSquared(const QPointF& p1, const QPointF& p2) {
    QPointF diff = p1 - p2;
    return QPointF::dotProduct(diff, diff);
}

Controller::Controller(int scene_width, int scene_height)
    : m_scene_width(scene_width), m_scene_height(scene_height)
{
    initializeBoundingBox(scene_width, scene_height);
    if (scene_width > 0 && scene_height > 0) {
        m_single_light_source_pos = QPointF(scene_width / 2.0, scene_height / 2.0);
        m_multiple_light_sources_pos.push_back(m_single_light_source_pos);
    }
}

void Controller::initializeBoundingBox(int width, int height) {
    std::vector<QPointF> vertices = {
        QPointF(0, 0), QPointF(800, 0),
        QPointF(800, 600), QPointF(0, 600)
    };
    m_bounding_box_polygon = Polygon(vertices);
}

const std::vector<Polygon>& Controller::getPolygons() const {
    return m_scene_polygons;
}

const std::vector<QPointF>& Controller::getBoundingBoxVertices() const {
    return m_bounding_box_polygon.getVertices();
}

void Controller::addPolygon(const Polygon& polygon) {
    m_scene_polygons.push_back(polygon);
}

void Controller::addVertexToLastPolygon(const QPointF& new_vertex) {
    if (!m_scene_polygons.empty()) {
        m_scene_polygons.back().addVertex(new_vertex);
    }
}

void Controller::updateLastPolygon(const QPointF& new_vertex) {
    if (!m_scene_polygons.empty()) {
        m_scene_polygons.back().updateLastVertex(new_vertex);
    }
}

QPointF Controller::getLightSource() const {
    return m_single_light_source_pos;
}

void Controller::setLightSource(const QPointF& light_source) {
    m_single_light_source_pos = light_source;
    if (m_multiple_light_sources_pos.empty()) {
        m_multiple_light_sources_pos.push_back(light_source);
    } else {
        m_multiple_light_sources_pos[0] = light_source;
    }
}

std::vector<QPointF> Controller::getLightSources() const {
    return m_multiple_light_sources_pos;
}
void Controller::setLightSources(const std::vector<QPointF>& sources) {
    m_multiple_light_sources_pos = sources;
    if (!sources.empty()) {
        m_single_light_source_pos = sources[0];
    } else if (m_scene_width > 0 && m_scene_height > 0) {
        m_single_light_source_pos = QPointF(m_scene_width/2.0, m_scene_height/2.0);
        m_multiple_light_sources_pos.push_back(m_single_light_source_pos);
    }
}

std::vector<Ray> Controller::castRaysFromSource(const QPointF& source_pos) const {
    std::vector<Ray> rays;
    const double angle_perturbation = 0.0001;

    auto add_rays_for_vertex = [&](const QPointF& vertex_pos) {
        if (pointDistanceSquared(source_pos, vertex_pos) < 1e-12) return;
        Ray main_ray(source_pos, vertex_pos);
        rays.push_back(main_ray);
        rays.push_back(main_ray.rotate(-angle_perturbation));
        rays.push_back(main_ray.rotate(angle_perturbation));
    };

    for (const auto& poly : m_scene_polygons) {
        if (poly.vertexCount() < 2 && poly.vertexCount() !=0 ) continue;
        for (const auto& vertex : poly.getVertices()) {
            add_rays_for_vertex(vertex);
        }
    }
    for (const auto& vertex : m_bounding_box_polygon.getVertices()) {
        add_rays_for_vertex(vertex);
    }

    if (rays.empty() && m_bounding_box_polygon.vertexCount() >= 3) {
        for (const auto& vertex : m_bounding_box_polygon.getVertices()) {
            if (pointDistanceSquared(source_pos, vertex) < 1e-12) continue;
            rays.push_back(Ray(source_pos, vertex));
        }
    }
    return rays;
}

std::vector<Ray> Controller::castRays() const {
    return castRaysFromSource(m_single_light_source_pos);
}

void Controller::intersectRays(std::vector<Ray>* rays_ptr) const {
    if (!rays_ptr) return;
    std::vector<Ray>& all_rays = *rays_ptr;

    std::vector<Polygon> all_obstacles = m_scene_polygons;
    all_obstacles.push_back(m_bounding_box_polygon);

    for (Ray& current_ray : all_rays) {
        QPointF ray_origin = current_ray.getBegin();
        QPointF closest_hit_point;
        double min_hit_dist_sq = std::numeric_limits<double>::max();
        bool hit_occurred = false;

        for (const auto& obstacle : all_obstacles) {
            std::optional<QPointF> intersection = obstacle.intersectRay(current_ray);
            if (intersection) {
                double dist_sq = pointDistanceSquared(ray_origin, *intersection);
                if (dist_sq > 1e-12 && dist_sq < min_hit_dist_sq) {
                    min_hit_dist_sq = dist_sq;
                    closest_hit_point = *intersection;
                    hit_occurred = true;
                }
            }
        }
        if (hit_occurred) {
            current_ray.setEnd(closest_hit_point);
        }
    }
}

void Controller::removeAdjacentRays(std::vector<Ray>* rays_ptr) const {
    if (!rays_ptr || rays_ptr->size() < 2) return;
    std::vector<Ray>& all_rays = *rays_ptr;

    std::sort(all_rays.begin(), all_rays.end(), Controller::compareRaysByAngle);

    std::vector<Ray> unique_rays_result;
    unique_rays_result.push_back(all_rays[0]);

    const double endpoint_distance_threshold_sq = 0.5 * 0.5;
    const double angle_threshold_rad = 0.00005;

    for (size_t i = 1; i < all_rays.size(); ++i) {
        const Ray& prev_ray = unique_rays_result.back();
        const Ray& current_ray = all_rays[i];

        double endpoint_dist_sq = pointDistanceSquared(current_ray.getEnd(), prev_ray.getEnd());
        double angle_diff = std::abs(current_ray.getAngle() - prev_ray.getAngle());
        if (angle_diff > M_PI) angle_diff = 2 * M_PI - angle_diff;

        if (endpoint_dist_sq > endpoint_distance_threshold_sq || angle_diff > angle_threshold_rad) {
            unique_rays_result.push_back(current_ray);
        }
    }
    all_rays = std::move(unique_rays_result);
}

bool Controller::compareRaysByAngle(const Ray& a, const Ray& b) {
    return a.getAngle() < b.getAngle();
}

Polygon Controller::createLightArea() const {
    std::vector<Ray> rays = castRays();
    if (rays.empty()) return Polygon();

    intersectRays(&rays);
    removeAdjacentRays(&rays);

    if (rays.size() < 3) return Polygon();

    std::sort(rays.begin(), rays.end(), Controller::compareRaysByAngle);
    std::vector<QPointF> vertices;
    vertices.reserve(rays.size());
    for (const auto& ray : rays) {
        vertices.push_back(ray.getEnd());
    }
    return Polygon(vertices);
}


Polygon Controller::calculateLightAreaForSource(const QPointF& source_pos) const {
    std::vector<Ray> rays = castRaysFromSource(source_pos);
    if (rays.empty()) return Polygon();

    bool is_source_inside_bbox =
        source_pos.x() > 0 && source_pos.x() < 800 &&
        source_pos.y() > 0 && source_pos.y() < 600;

    if (!is_source_inside_bbox) {
        return Polygon();
    }

    intersectRays(&rays);
    removeAdjacentRays(&rays);

    if (rays.size() < 3) return Polygon();

    std::sort(rays.begin(), rays.end(), Controller::compareRaysByAngle);
    std::vector<QPointF> vertices;
    vertices.reserve(rays.size());
    for (const auto& ray : rays) {
        vertices.push_back(ray.getEnd());
    }
    return Polygon(vertices);
}


std::vector<Polygon> Controller::calculateAllLightAreasForMultipleSources() {
    std::vector<Polygon> light_areas;
    if (m_multiple_light_sources_pos.empty()) {
        if (m_scene_width > 0 && m_scene_height > 0) {
            m_multiple_light_sources_pos.push_back(QPointF(m_scene_width / 2.0, m_scene_height / 2.0));
        } else {
            return light_areas;
        }
    }

    for (const auto& source : m_multiple_light_sources_pos) {
        Polygon area = calculateLightAreaForSource(source);
        if (area.vertexCount() >= 3) {
            light_areas.push_back(area);
        }
    }
    return light_areas;
}

