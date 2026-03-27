#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include "polygon.h"

class Controller
{
private:
    std::vector<Polygon> polygons;
    std::vector<QPointF> static_light_sources;
    QPointF light_source;
    std::vector<QPointF> lights; // околомышные
    std::vector<std::pair<QPointF, float>> bounded_lights;

public:
    Controller() {
        lights.resize(10);
    }
    const std::vector<Polygon>& GetPolygons();
    void AddPolygon(const Polygon&);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);
    std::vector<QPointF>& getLights();
    std::vector<QPointF>& getL();
    std::vector<std::pair<QPointF, float>>& getBoundedL() {
        return bounded_lights;
    }
    void setBoundedL(std::vector<std::pair<QPointF, float>>& l) {
        bounded_lights = l;
    }
    void setLights(const std::vector<QPointF>&);
    QPointF& getLight();
    void setLight(const QPointF&);
    short mode = 0; // 0 - light, 1 - polygons 2 - bounded
    std::vector<Ray> CastRaysLight();
    std::vector<std::vector<Ray>> CastRaysLights();

    std::vector<std::vector<Ray>> CastRaysBoundedLights();
    void IntersectRaysB(std::vector<Ray>* rays, float len);

    void IntersectRays(std::vector<Ray>* rays);
    void RemoveAdjacentRays(std::vector<Ray>* rays);
    void RemoveAdjacentRaysStatic(std::vector<Ray>* rays);
    Polygon CreateLightArea(std::vector<Ray>* rays);

    std::vector<std::vector<Ray>> CastRaysL();

};

#endif // CONTROLLER_H
