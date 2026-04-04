#pragma once

#include <QPointF>
#include <QSizeF>

#include <vector>

#include "polygon.h"
#include "ray.h"

class Controller {
public:
    Controller();

    const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);
    void FinalizeLastPolygon();

    const QPointF& GetLightSource() const;
    void SetLightSource(const QPointF& light_source);

    void SetSceneSize(const QSizeF& size);
    bool HasActivePolygon() const;
    std::vector<QPointF> GetLightSources() const;

    std::vector<Ray> CastRays() const;
    void IntersectRays(std::vector<Ray>* rays) const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;
    Polygon CreateLightArea() const;
    Polygon CreateLightAreaForSource(const QPointF& source) const;

private:
    std::vector<Polygon> AllPolygons() const;
    std::vector<Ray> CastRaysForSource(const QPointF& source) const;
    void IntersectRaysForSource(const QPointF& source, std::vector<Ray>* rays) const;
    double SceneRange() const;

    std::vector<Polygon> polygons_;
    QPointF light_source_;
    QSizeF scene_size_;
    Polygon boundary_;
    std::vector<QPointF> dynamic_light_offsets_;
    bool has_active_polygon_;
};
