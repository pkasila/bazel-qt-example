#pragma once

#include "polygon.h"

#include <QPointF>

#include <vector>

class Controller {
public:
    void AddPolygon(const Polygon& polygon);

    void AddVertexToLastPolygon(const QPointF& vertex);

    void UpdateLastPolygon(const QPointF& vertex);

    void FinishLastPolygon();

    void SetLightSource(const QPointF& source);

    const QPointF& GetLightSource() const;

    const std::vector<Polygon>& GetPolygons() const;

    std::vector<Ray> CastRays() const;

    void IntersectRays(std::vector<Ray>* rays) const;

    Polygon CreateLightArea() const;

private:
    std::vector<Polygon> polygons_;

    QPointF light_source_;
};
