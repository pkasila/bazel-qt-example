#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "Polygon.h"

class Controller {
public:
    const std::vector<Polygon>& GetPolygons() const { return m_polygons; }
    void AddPolygon(const Polygon& p) { m_polygons.push_back(p); }
    void AddVertexToLastPolygon(const QPointF& v) { if(!m_polygons.empty()) m_polygons.back().AddVertex(v); }
    void UpdateLastPolygon(const QPointF& v) { if(!m_polygons.empty()) m_polygons.back().UpdateLastVertex(v); }
    void setLightSource(const QPointF& p) { m_light_source = p; }
    QPointF getLightSource() const { return m_light_source; }
    
    std::vector<Ray> CastRays();
    void IntersectRays(std::vector<Ray>* rays);
    void RemoveAdjacentRays(std::vector<Ray>* rays);
    Polygon CreateLightArea();
private:
    std::vector<Polygon> m_polygons;
    QPointF m_light_source;
};
#endif