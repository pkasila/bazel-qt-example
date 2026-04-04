#include "raycaster_controller.h"
#include <cmath>
#include <algorithm>

const std::vector<Polygon>& Controller::GetPolygons() const {
    return m_polygons;
}

void Controller::AddPolygon(const Polygon& p) {
    m_polygons.push_back(p);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (!m_polygons.empty()) {
        m_polygons.back().AddVertex(new_vertex);
    }
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!m_polygons.empty()) {
        m_polygons.back().UpdateLastVertex(new_vertex);
    }
}

QPointF Controller::getLightSource() const {
    return m_light_source;
}

void Controller::setLightSource(const QPointF& source) {
    m_light_source = source;
}

void Controller::RemoveLastVertexFromLastPolygon() {
    if (!m_polygons.empty()) {
        m_polygons.back().RemoveLastVertex();
    }
}

std::vector<Ray> Controller::CastRays(const QPointF& light_pos) const {
    std::vector<Ray> rays;

    for (const auto& poly : m_polygons) {
        for (const auto& v : poly.getVertices()) {
            double angle = std::atan2(v.y() - light_pos.y(), v.x() - light_pos.x());
            double length = 3000.0;
            
            auto make_ray = [&](double a) {
                QPointF end(light_pos.x() + length * std::cos(a),
                            light_pos.y() + length * std::sin(a));
                return Ray(light_pos, end, a);
            };

            rays.push_back(make_ray(angle - 0.0001));
            rays.push_back(make_ray(angle));
            rays.push_back(make_ray(angle + 0.0001));
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    for (auto& ray : *rays) {
        QPointF closest_end = ray.getEnd();
        double min_dist = std::hypot(closest_end.x() - ray.getBegin().x(), 
                                     closest_end.y() - ray.getBegin().y());

        for (const auto& poly : m_polygons) {
            auto intersect = poly.IntersectRay(ray);
            if (intersect.has_value()) { 
                double dist = std::hypot(intersect.value().x() - ray.getBegin().x(), 
                                         intersect.value().y() - ray.getBegin().y());
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_end = intersect.value();
                }
            }
        }
        ray.setEnd(closest_end);
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    if (rays->empty()) return;

    std::sort(rays->begin(), rays->end(),[](const Ray& a, const Ray& b) {
        return a.getAngle() < b.getAngle();
    });

    std::vector<Ray> filtered;
    filtered.push_back((*rays)[0]);
    
    for (size_t i = 1; i < rays->size(); ++i) {
        QPointF p1 = filtered.back().getEnd();
        QPointF p2 = (*rays)[i].getEnd();
        
        double dist = std::hypot(p1.x() - p2.x(), p1.y() - p2.y());
        if (dist > 1.0) {
            filtered.push_back((*rays)[i]);
        }
    }
    *rays = filtered;
}

void Controller::AddStaticLight(const QPointF& pos) {
    m_static_lights.push_back(pos);
}

const std::vector<QPointF>& Controller::GetStaticLights() const {
    return m_static_lights;
}

void Controller::ClearPolygons() {
    if (m_polygons.size() > 1) {
        m_polygons.erase(m_polygons.begin() + 1, m_polygons.end());
    }
    m_static_lights.clear();
}

std::vector<Polygon> Controller::CreateLightAreas() const {
    std::vector<Polygon> areas;

    std::vector<QPointF> all_sources = { m_light_source };
    all_sources.insert(all_sources.end(), m_static_lights.begin(), m_static_lights.end());

    for (const auto& base_pos : all_sources) {
        std::vector<QPointF> cluster = {
            base_pos,
            base_pos + QPointF(-4, -4),
            base_pos + QPointF(4, -4),
            base_pos + QPointF(-4, 4),
            base_pos + QPointF(4, 4)
        };

        for (const auto& pos : cluster) {
            auto rays = CastRays(pos);
            IntersectRays(&rays);
            RemoveAdjacentRays(&rays);

            std::vector<QPointF> points;
            for (const auto& r : rays) {
                points.push_back(r.getEnd());
            }
            areas.push_back(Polygon(points));
        }
    }

    return areas;
}