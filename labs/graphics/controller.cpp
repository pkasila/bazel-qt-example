#include "controller.h"
#include <cmath>

const std::vector<Polygon>& Controller::GetPolygons() const { return m_polygons; }
void Controller::AddPolygon(const Polygon& p) { m_polygons.push_back(p); }
void Controller::AddVertexToLastPolygon(const QPointF& v) { if(!m_polygons.empty()) m_polygons.back().AddVertex(v); }
void Controller::UpdateLastPolygon(const QPointF& v) { if(!m_polygons.empty()) m_polygons.back().UpdateLastVertex(v); }
QPointF Controller::getLightSource() const { return m_lightSource; }
void Controller::setLightSource(const QPointF& pos) { m_lightSource = pos; }

void Controller::UpdateFrame(int w, int h) {
    Polygon b;
    b.AddVertex({0,0}); b.AddVertex({(double)w,0}); b.AddVertex({(double)w,(double)h}); b.AddVertex({0,(double)h});
    if (m_polygons.empty()) m_polygons.push_back(b); else m_polygons[0] = b;
}

void Controller::ClearPolygons() { if (m_polygons.size() > 1) m_polygons.erase(m_polygons.begin() + 1, m_polygons.end()); }

std::vector<Ray> Controller::CastRays(const QPointF& src) {
    std::vector<Ray> rays;
    for (const auto& poly : m_polygons) {
        for (const auto& v : poly.getVertices()) {
            double a = std::atan2(v.y() - src.y(), v.x() - src.x());
            for (double d : {-0.0001, 0.0, 0.0001}) {
                double ang = a + d;
                rays.push_back(Ray(src, QPointF(src.x() + std::cos(ang) * 5000, src.y() + std::sin(ang) * 5000), ang));
            }
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) {
    for (auto& r : *rays) {
        for (const auto& poly : m_polygons) {
            auto hit = poly.IntersectRay(r);
            if (hit) {
                double d1 = std::pow(hit->x() - r.getBegin().x(), 2) + std::pow(hit->y() - r.getBegin().y(), 2);
                double d2 = std::pow(r.getEnd().x() - r.getBegin().x(), 2) + std::pow(r.getEnd().y() - r.getBegin().y(), 2);
                if (d1 < d2) r.setEnd(*hit);
            }
        }
    }
}

Polygon Controller::CreateLightArea(const QPointF& src) {
    auto rays = CastRays(src);
    IntersectRays(&rays);
    std::sort(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) { return a.getAngle() < b.getAngle(); });
    std::vector<QPointF> v;
    for (const auto& r : rays) v.push_back(r.getEnd());
    return Polygon(v);
}