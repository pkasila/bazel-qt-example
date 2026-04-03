#pragma once

#include <vector>
#include <optional>
#include <QPoint>
#include "polygon.h"
#include "ray.h"

class Controller {
public:
    Controller(int width, int height);
    
    const std::vector<Polygon>& GetPolygons() const { return polygons_; }
    std::vector<Polygon>& GetPolygons() { return polygons_; }
    
    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPoint& new_vertex);
    void UpdateLastPolygon(const QPoint& new_vertex);
    void FinishCurrentPolygon();
    
    // Проверка коллизий
    bool IsPointInsideAnyPolygon(const QPoint& point) const;
    bool DoPolygonsIntersect(const Polygon& polygon) const;
    bool IsPolygonValid(const Polygon& polygon) const;
    
    QPoint getLightSource() const { return light_source_; }
    void setLightSource(const QPoint& source) { 
        if (!IsPointInsideAnyPolygon(source)) {
            light_source_ = source;
        }
    }
    
    std::vector<QPoint> getMultipleLightSources() const { return multiple_light_sources_; }
    void updateMultipleLightsCenter(const QPoint& center);
    
    // Статические источники
    void addStaticLight(const QPoint& position);
    void removeLastStaticLight();
    const std::vector<QPoint>& getStaticLights() const { return static_lights_; }
    
    Polygon CreateLightArea(const QPoint& source) const;
    std::vector<Polygon> CreateMultipleLightAreas() const;
    std::vector<Polygon> CreateStaticLightAreas() const;
    
    void ClearCurrentPolygon() { currentPolygonVertices_.clear(); }
    bool HasCurrentPolygon() const { return !currentPolygonVertices_.empty(); }
    const std::vector<QPoint>& GetCurrentPolygonVertices() const { return currentPolygonVertices_; }
    
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    
private:
    std::vector<Polygon> polygons_;
    std::vector<QPoint> currentPolygonVertices_;
    QPoint light_source_;
    std::vector<QPoint> multiple_light_sources_;
    std::vector<QPoint> static_lights_;
    int width_;
    int height_;
    double circle_radius_;
    
    QPoint IntersectWithBoundary(const QPoint& source, double angle) const;
    bool IsPointInsidePolygon(const QPoint& point, const Polygon& polygon) const;
    bool DoSegmentsIntersect(const QPoint& a1, const QPoint& a2, const QPoint& b1, const QPoint& b2) const;
    QPoint FindNearestPointOutsidePolygon(const QPoint& point) const;
    QPoint ClosestPointOnSegment(const QPoint& p, const QPoint& a, const QPoint& b) const;
};