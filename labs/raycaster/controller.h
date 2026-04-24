#pragma once

#include <vector>
#include <QPoint>
#include <QPointF>
#include <QRectF>
#include "ray.h"
#include "polygon.h"

class Controller {
public:
    Controller();
    
    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const { return polygons_; }
    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPoint& new_vertex);
    void UpdateLastPolygon(const QPoint& new_vertex);
    
    [[nodiscard]] QPoint GetLightSource() const;
    void SetLightSource(const QPoint& pos);
    
    [[nodiscard]] std::vector<Ray> CastRays() const;
    void IntersectRays(std::vector<Ray>* rays) const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;
    [[nodiscard]] Polygon CreateLightArea() const;
    
    [[nodiscard]] const std::vector<QPointF>& GetLightSources() const { return light_sources_; }
    void SetLightCluster(const QPointF& center);
    [[nodiscard]] std::vector<Polygon> CreateLightAreas() const;
    
    void SetBounds(const QRectF& bounds);
    
private:
    std::vector<Polygon> polygons_;
    std::vector<QPointF> light_sources_;
    
    [[nodiscard]] static double VectorAngle(const QPointF& from, const QPointF& to);
    [[nodiscard]] static double Distance(const QPointF& a, const QPointF& b);
    std::vector<Ray> CastRaysForLight(const QPointF& light) const;
};