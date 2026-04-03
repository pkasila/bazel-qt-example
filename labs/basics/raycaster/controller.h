#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"
#include <vector>

class Controller
{
public:
    Controller();

    // Polygon management
    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    // Polygon access
    std::vector<Polygon>& GetPolygons();
    const std::vector<Polygon>& GetPolygons() const;
    bool HasPolygons() const;

    // Light source management
    void SetLightSource(const QPointF& new_source);
    const QPointF& GetLightSource() const;

    bool IsPointInsidePolygon(const QPointF& point, const std::vector<QPointF>& polygon);


    std::vector<Ray> CastRays(const QPointF& light_s);
    void IntersectRays(std::vector<Ray>* rays);
    void RemoveAdjacentRays(std::vector<Ray>* rays);
    std::vector<Polygon> CreateLightArea();
    void GenerateShades();

private:
    std::vector<Polygon> polygons_;
    QPointF light_source_;
    std::vector<QPointF> light_sources_shades;
};

#endif // CONTROLLER_H
