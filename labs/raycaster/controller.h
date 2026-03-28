#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <QPoint>
#include "polygon.h"
#include "ray.h"

class Controller {
public:
    Controller();

    const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPoint& new_vertex);
    void UpdateLastPolygon(const QPoint& new_vertex);

    const QPoint& GetLightSource() const { return light_source_; }
    void SetLightSource(const QPoint& light_source) { light_source_ = light_source; }

    std::vector<Ray> CastRays();
    void IntersectRays(std::vector<Ray>* rays);
    void RemoveAdjacentRays(std::vector<Ray>* rays);
    Polygon CreateLightArea();

    std::vector<Polygon> CreateLightAreas();
    const std::vector<QPoint>& GetLightSources() const { return light_sources_; }
    void SetLightSources(const std::vector<QPoint>& light_sources) { light_sources_ = light_sources; }
    
private:
    std::vector<Polygon> polygons_;
    QPoint light_source_;
    std::vector<QPoint> light_sources_;
    
    void InitializeBoundaryPolygon();
    double CalculateRayAngle(const Ray& ray) const;
};

#endif // CONTROLLER_H
