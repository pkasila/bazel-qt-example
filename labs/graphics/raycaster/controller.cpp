#include "controller.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <algorithm>


const std::vector<Polygon>& Controller::getPolygons() const { return m_polygons; }

void Controller::addPolygon(const Polygon& polygon) { m_polygons.push_back(polygon); }
bool comp(Ray a, Ray b){
    return a.getAngle() > b.getAngle();
}

void Controller::addVertexToLastPolygon(const QPointF& vertex) {
    if (m_polygons.size() > 1) m_polygons.back().addVertex(vertex);
}

void Controller::updateLastPolygonVertex(const QPointF& new_vertex) {
    if ( m_polygons.size() > 1 && !m_polygons.back().getVertices().empty()) {
        m_polygons.back().updateLastVertex(new_vertex);
    }
}

QPointF Controller::getLightSource() const { return m_lightSource; }

bool Controller::setLightSource(const QPointF& pos) {
    if (!m_polygons.empty() && m_polygons[0].containsPoint(pos)) {
        m_lightSource = pos;
        return true;
    }
    return false;
}

void Controller::updateBoundingPolygon(const QPointF& topLeft, const QPointF& bottomRight) {
    std::vector<QPointF> vertices = { topLeft, {bottomRight.x(), topLeft.y()}, bottomRight, {topLeft.x(), bottomRight.y()} };
    if (m_polygons.empty()) m_polygons.emplace_back(vertices);
    else m_polygons[0] = Polygon(vertices);
}

std::vector<Ray> Controller::castRays() const {
    std::vector<Ray> rays;
    for (const auto& polygon : m_polygons) {
        for (const auto& vertex : polygon.getVertices()) {
            Ray mainRay(m_lightSource, vertex);
            rays.push_back(mainRay);
            rays.push_back(mainRay.rotate(0.001));
            rays.push_back(mainRay.rotate(-0.001));
        }
    }
    return rays;
}

void Controller::intersectRays(std::vector<Ray>* rays) const {
    for (auto& ray : *rays) {
        QPointF closestEnd = ray.getEnd();
        double minSqDist = std::numeric_limits<double>::max();
        for (const auto& polygon : m_polygons) {
            auto intersection = polygon.intersectRay(ray);
            if (intersection.has_value()) {
                const QPointF delta = intersection.value() - ray.getBegin();
                const double sqDist = delta.x() * delta.x() + delta.y() * delta.y();

                if (sqDist < minSqDist) {
                    minSqDist = sqDist;
                    closestEnd = intersection.value();
                }
            }
        }
        ray.setEnd(closestEnd);
    }
}

void Controller::removeAdjacentRays(std::vector<Ray>* rays) const {
    const double epsilon = 1e-8;
    auto it = std::unique(rays->begin(), rays->end(), [epsilon](const Ray& a, const Ray& b) {
        QPointF delta = a.getEnd() - b.getEnd();
        return (delta.x() * delta.x() + delta.y() * delta.y()) < epsilon;
    });
    rays->erase(it, rays->end());
}

Polygon Controller::createLightArea()  {
    std::vector<Ray> rays = castRays();
    intersectRays(&rays);
    removeAdjacentRays(&rays);
    std::sort(rays.begin(), rays.end(), comp);
    std::vector<QPointF> vertices;
    for (const auto& ray : rays) vertices.push_back(ray.getEnd());
    if (!vertices.empty()) vertices.push_back(vertices.front());
    return Polygon(vertices);
}

Polygon* Controller::curpol(){
    return& cur_poly;
}
