#pragma once

#include <vector>
#include <QPoint>
#include <QColor>
#include "Polygon.h"

class Controller {
public:
    Controller();
    Controller(const QColor& color);

    const std::vector<Polygon>& GetPolygons() const;

    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    QPointF GetLightSource() const;
    QColor GetColor() const;
    void SetLightSource(const QPointF& point);

    std::vector<Ray> CastRays();
    void IntersectRays(std::vector<Ray>* rays);
    void RemoveAdjacentRays(std::vector<Ray>* rays);
    Polygon CreateLightArea();

    void ClearPolygons();

private:
    std::vector<Polygon> polygons_;
    QPointF light_source_;
    QColor color_;
};
