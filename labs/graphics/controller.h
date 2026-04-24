#pragma once
#include <vector>
#include <algorithm>
#include "polygon.h"

class Controller {
public:
    const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& p);
    void AddVertexToLastPolygon(const QPointF& v);
    void UpdateLastPolygon(const QPointF& v);
    void UpdateFrame(int w, int h);
    void ClearPolygons();
    QPointF getLightSource() const;
    void setLightSource(const QPointF& pos);
    std::vector<Ray> CastRays(const QPointF& source);
    void IntersectRays(std::vector<Ray>* rays);
    Polygon CreateLightArea(const QPointF& source);

private:
    std::vector<Polygon> m_polygons;
    QPointF m_lightSource = {100, 100};
};