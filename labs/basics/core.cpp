#include "core.h"
#include <cmath>
#include <algorithm>

const double PI = 3.14159265358979323846;
const double RAY_LENGTH = 3000.0;

// ================= RAY =================
Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : m_begin(begin), m_end(end), m_angle(angle) {}

QPointF Ray::getBegin() const { return m_begin; }
void Ray::setBegin(const QPointF& begin) { m_begin = begin; }
QPointF Ray::getEnd() const { return m_end; }
void Ray::setEnd(const QPointF& end) { m_end = end; }
double Ray::getAngle() const { return m_angle; }
void Ray::setAngle(double angle) { m_angle = angle; }

Ray Ray::Rotate(double angleOffset) const {
    double newAngle = m_angle + angleOffset;
    while (newAngle > PI) newAngle -= 2 * PI;
    while (newAngle < -PI) newAngle += 2 * PI;
    QPointF newEnd(m_begin.x() + std::cos(newAngle) * RAY_LENGTH,
                   m_begin.y() + std::sin(newAngle) * RAY_LENGTH);
    return Ray(m_begin, newEnd, newAngle);
}

// ================= POLYGON =================
Polygon::Polygon(const std::vector<QPointF>& vertices) : m_vertices(vertices) {}
const std::vector<QPointF>& Polygon::GetVertices() const { return m_vertices; }
void Polygon::AddVertex(const QPointF& vertex) { m_vertices.push_back(vertex); }
void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (!m_vertices.empty()) m_vertices.back() = new_vertex;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (m_vertices.size() < 2) return std::nullopt;
    std::optional<QPointF> closest_intersection;
    double min_t = 1.0;
    QPointF p = ray.getBegin();
    QPointF r = Subtract(ray.getEnd(), ray.getBegin());

    for (size_t i = 0; i < m_vertices.size(); ++i) {
        QPointF q = m_vertices[i];
        QPointF s = Subtract(m_vertices[(i + 1) % m_vertices.size()], q);
        double r_cross_s = CrossProduct(r, s);
        QPointF q_minus_p = Subtract(q, p);
        
        if (std::abs(r_cross_s) < 1e-8) continue;
        double t = CrossProduct(q_minus_p, s) / r_cross_s;
        double u = CrossProduct(q_minus_p, r) / r_cross_s;

        if (t >= 0.0 && t <= 1.0 && u >= 0.0 && u <= 1.0) {
            if (t < min_t) {
                min_t = t;
                closest_intersection = QPointF(p.x() + t * r.x(), p.y() + t * r.y());
            }
        }
    }
    return closest_intersection;
}

bool Polygon::ContainsPoint(const QPointF& pt) const {
    if (m_vertices.size() < 3) return false;
    bool inside = false;
    for (size_t i = 0, j = m_vertices.size() - 1; i < m_vertices.size(); j = i++) {
        QPointF pi = m_vertices[i];
        QPointF pj = m_vertices[j];
        if (((pi.y() > pt.y()) != (pj.y() > pt.y())) &&
            (pt.x() < (pj.x() - pi.x()) * (pt.y() - pi.y()) / (pj.y() - pi.y()) + pi.x())) {
            inside = !inside;
        }
    }
    return inside;
}

void Polygon::MakeClockwise() {
    if (m_vertices.size() < 3) return;
    double sum = 0;
    for (size_t i = 0; i < m_vertices.size(); ++i) {
        QPointF p1 = m_vertices[i];
        QPointF p2 = m_vertices[(i + 1) % m_vertices.size()];
        sum += (p2.x() - p1.x()) * (p2.y() + p1.y());
    }
    if (sum > 0) {
        std::reverse(m_vertices.begin(), m_vertices.end());
    }
}

// ================= CONTROLLER =================
const std::vector<Polygon>& Controller::GetPolygons() const { return m_polygons; }
void Controller::AddPolygon(const Polygon& p) { m_polygons.push_back(p); }
void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (!m_polygons.empty()) m_polygons.back().AddVertex(new_vertex);
}
void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!m_polygons.empty()) m_polygons.back().UpdateLastVertex(new_vertex);
}

void Controller::FinalizeLastPolygon() {
    if (m_polygons.size() > 1) {
        m_polygons.back().MakeClockwise();
        
        auto it = m_lights.begin();
        while (it != m_lights.end()) {
            if (m_polygons.back().ContainsPoint(it->pos)) {
                it = m_lights.erase(it);
            } else {
                ++it;
            }
        }
    }
}

const std::vector<Light>& Controller::GetLights() const { return m_lights; }
void Controller::AddLight(const QPointF& pos, const QColor& color) { m_lights.push_back({pos, color}); }
void Controller::RemoveLight(size_t index) {
    if (index < m_lights.size()) m_lights.erase(m_lights.begin() + index);
}
void Controller::SetLightPos(size_t index, const QPointF& pos) {
    if (index < m_lights.size()) m_lights[index].pos = pos;
}
void Controller::SetLightColor(size_t index, const QColor& color) {
    if (index < m_lights.size()) m_lights[index].color = color;
}

bool Controller::IsPointInAnyPolygon(const QPointF& pt) const {
    for (size_t i = 1; i < m_polygons.size(); ++i) {
        if (m_polygons[i].ContainsPoint(pt)) return true;
    }
    return false;
}

std::vector<Ray> Controller::CastRays(const QPointF& light_source) const {
    std::vector<Ray> rays;
    for (const auto& poly : m_polygons) {
        for (const auto& vertex : poly.GetVertices()) {
            double angle = std::atan2(vertex.y() - light_source.y(), vertex.x() - light_source.x());
            QPointF end(light_source.x() + std::cos(angle) * RAY_LENGTH,
                        light_source.y() + std::sin(angle) * RAY_LENGTH);
            Ray baseRay(light_source, end, angle);
            rays.push_back(baseRay.Rotate(-0.0001));
            rays.push_back(baseRay);
            rays.push_back(baseRay.Rotate(0.0001));
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    for (auto& ray : *rays) {
        double min_dist = Distance(ray.getBegin(), ray.getEnd());
        QPointF closest_point = ray.getEnd();
        for (const auto& poly : m_polygons) {
            auto intersection = poly.IntersectRay(ray);
            if (intersection.has_value()) {
                double dist = Distance(ray.getBegin(), intersection.value());
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_point = intersection.value();
                }
            }
        }
        ray.setEnd(closest_point);
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    if (rays->empty()) return;
    std::vector<Ray> filtered;
    filtered.push_back((*rays)[0]);
    for (size_t i = 1; i < rays->size(); ++i) {
        if (Distance(filtered.back().getEnd(), (*rays)[i].getEnd()) > 1.0) {
            filtered.push_back((*rays)[i]);
        }
    }
    *rays = filtered;
}

Polygon Controller::CreateLightArea(const QPointF& light_source) const {
    auto rays = CastRays(light_source);
    IntersectRays(&rays);
    std::sort(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) {
        return a.getAngle() < b.getAngle();
    });
    RemoveAdjacentRays(&rays);
    std::vector<QPointF> light_vertices;
    for (const auto& ray : rays) {
        light_vertices.push_back(ray.getEnd());
    }
    return Polygon(light_vertices);
}