#pragma once

#include "raycaster_polygon.h"
#include <vector>

class Controller {
public:
    Controller() = default;

    const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& p);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    QPointF getLightSource() const;
    void setLightSource(const QPointF& source);

    std::vector<Ray> CastRays(const QPointF& light_pos) const; 
    void IntersectRays(std::vector<Ray>* rays) const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;
    std::vector<Polygon> CreateLightAreas() const;
    void RemoveLastVertexFromLastPolygon();

    void ClearPolygons();

    void AddStaticLight(const QPointF& pos);
    const std::vector<QPointF>& GetStaticLights() const;

private:
    std::vector<Polygon> m_polygons;
    QPointF m_light_source;
    std::vector<QPointF> m_static_lights;
};