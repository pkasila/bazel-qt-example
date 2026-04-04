#pragma once

#include <cstddef>
#include <vector>

#include <QPointF>

#include "labs/raycaster/polygon.h"
#include "labs/raycaster/ray.h"

class Controller {
public:
    Controller();

    const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    const QPointF& GetLightSource() const;
    void SetLightSource(const QPointF& light_source);

    std::vector<Ray> CastRays() const;
    std::vector<Ray> CastRays(const QPointF& source) const;
    void IntersectRays(std::vector<Ray>* rays) const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;
    Polygon CreateLightArea() const;
    Polygon CreateLightArea(const QPointF& source) const;
    std::vector<Polygon> CreateLightAreas() const;

    void FinishActivePolygon();
    bool HasActivePolygon() const;
    std::size_t GetActivePolygonIndex() const;

    void SetSceneBounds(const QPointF& top_left, const QPointF& bottom_right);
    std::vector<QPointF> GetLightSources() const;

    const Polygon& GetBoundaryPolygon() const;

private:
    QPointF ClampToScene(const QPointF& point) const;
    bool IsObstaclePolygon(std::size_t index) const;

    std::vector<Polygon> polygons_;
    Polygon boundary_polygon_;
    QPointF light_source_;
    QPointF scene_top_left_;
    QPointF scene_bottom_right_;
    bool has_scene_rect_;
    bool has_active_polygon_;
    std::size_t active_polygon_index_;
};
