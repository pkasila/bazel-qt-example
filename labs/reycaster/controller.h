#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QPointF>
#include <vector>
#include "polygon.h"
#include "ray.h"

class Controller {
public:
    Controller();
    
    // Геттеры и сеттеры
    const std::vector<Polygon>& GetPolygons() const { return polygons_; }
    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);
    
    QPointF getLightSource() const { return light_source_; }
    void setLightSource(const QPointF& light_source) { light_source_ = light_source; }
    
    // Методы для работы с лучами
    std::vector<Ray> CastRays();
    void IntersectRays(std::vector<Ray>* rays);
    void RemoveAdjacentRays(std::vector<Ray>* rays);
    Polygon CreateLightArea();
    
    // Для множественных источников света (задание 3)
    void AddLightSource(const QPointF& light_source);
    const std::vector<QPointF>& GetLightSources() const { return light_sources_; }
    void setLightSources(const std::vector<QPointF>& sources) { light_sources_ = sources; }
    std::vector<Polygon> CreateLightAreas();
    
    // Для создания граничного многоугольника
    void CreateBoundaryPolygon(int width, int height);
    
private:
    std::vector<Polygon> polygons_;
    QPointF light_source_;
    std::vector<QPointF> light_sources_; // Для задания 3
    
    double CalculateRayAngle(const Ray& ray) const;
};

#endif // CONTROLLER_H
