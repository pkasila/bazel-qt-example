#pragma once

#include "labs/raycaster/core/polygon.h"
#include "labs/raycaster/core/ray.h"

#include <QPointF>
#include <vector>

namespace raycaster {

class Controller {
   public:
    Controller();

    const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    const QPointF& GetLightSource() const;
    void SetLightSource(const QPointF& light_source);
    bool AreSoftShadowsEnabled() const;
    void SetSoftShadowsEnabled(bool enabled);

    void SetSceneBounds(double left, double top, double right, double bottom);

    bool IsDrawingPolygon() const;
    bool FinalizeLastPolygon();
    void Clear();

    std::vector<Ray> CastRays() const;
    void IntersectRays(std::vector<Ray>* rays) const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;
    Polygon CreateLightArea() const;

    std::vector<QPointF> GetDynamicLightSources() const;
    const std::vector<QPointF>& GetStaticLights() const;
    void AddStaticLight(const QPointF& light_source);
    void ClearStaticLights();

    Polygon CreateLightAreaForSource(const QPointF& light_source) const;
    bool IsPointInsideAnyObstacle(const QPointF& point) const;

   private:
    std::vector<const Polygon*> GetObstaclePolygons() const;
    std::vector<Ray> CastRaysFrom(const QPointF& light_source) const;
    bool CanMoveDynamicLightsTo(const QPointF& center) const;
    bool IsInsideScene(const QPointF& point, double padding = 0.0) const;
    QPointF ClampToScene(const QPointF& point, double padding = 0.0) const;
    bool RelocateDynamicLightsNear(const QPointF& preferred_center);
    static std::vector<QPointF> SanitizePolygonVertices(const std::vector<QPointF>& vertices);

    std::vector<Polygon> polygons_;
    QPointF light_source_;
    double scene_left_;
    double scene_top_;
    double scene_right_;
    double scene_bottom_;
    Polygon boundary_polygon_;
    bool is_drawing_polygon_;
    bool soft_shadows_enabled_;
    std::vector<QPointF> dynamic_light_offsets_;
    std::vector<QPointF> static_lights_;
};

}  // namespace raycaster
