#pragma once

#include <QtCore/QPointF>
#include <QtCore/QRectF>

#include <optional>
#include <vector>

#include "labs/raycaster/polygon.h"
#include "labs/raycaster/ray.h"

namespace raycaster {

class Controller {
public:
    Controller();

    const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    const std::optional<Polygon>& GetCurrentPolygon() const;
    void FinalizeLastPolygon();
    bool HasCurrentPolygon() const;

    const QPointF& GetLightSource() const;
    void SetLightSource(const QPointF& light_source);

    std::vector<QPointF> GetLightSources() const;

    void SetSceneRect(const QRectF& scene_rect);
    Polygon CreateLightArea() const;
    std::vector<Polygon> CreateSoftLightAreas() const;

    std::vector<Ray> CastRays() const;
    void IntersectRays(std::vector<Ray>* rays) const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;

private:
    std::vector<Polygon> GetOccluders() const;
    std::vector<Ray> CastRaysFrom(const QPointF& source) const;
    void IntersectRaysFrom(const QPointF& source, std::vector<Ray>* rays) const;
    void RemoveAdjacentRaysFrom(const QPointF& source, std::vector<Ray>* rays) const;
    Polygon CreateLightAreaFrom(const QPointF& source) const;
    double GetMaxRayLength() const;

    std::vector<Polygon> polygons_;
    std::optional<Polygon> current_polygon_;
    std::optional<Polygon> boundary_polygon_;
    QPointF light_source_ = QPointF(300.0, 200.0);
    std::vector<QPointF> soft_light_offsets_;
    QRectF scene_rect_;
};

}  // namespace raycaster
