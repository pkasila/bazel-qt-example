#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"
#include "ray.h"

#include <QPointF>
#include <numbers>
#include <optional>
#include <vector>

class Controller {
   public:
    Controller(int width, int height);

    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPointF& vertex);
    void UpdateLastPolygon(const QPointF& vertex);
    void FinishCurrentPolygon();

    [[nodiscard]] QPointF GetLightSource() const;
    void SetLightSource(const QPointF& source);

    [[nodiscard]] std::vector<Ray> CastRays() const;
    void IntersectRays(std::vector<Ray>* rays) const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;
    [[nodiscard]] Polygon CreateLightArea() const;

    [[nodiscard]] std::vector<Ray> CastRays(const QPointF& source) const;
    [[nodiscard]] Polygon CreateLightArea(const QPointF& source) const;

    [[nodiscard]] std::vector<QPointF> GetMultipleLightSources() const;
    void UpdateMultipleLightsCenter(const QPointF& center);
    [[nodiscard]] std::vector<Polygon> CreateMultipleLightAreas() const;

    void AddStaticLight(const QPointF& position);
    void RemoveLastStaticLight();
    [[nodiscard]] const std::vector<QPointF>& GetStaticLights() const;
    [[nodiscard]] std::vector<Polygon> CreateStaticLightAreas() const;

    [[nodiscard]] bool IsPointInsideAnyPolygon(const QPointF& point) const;
    [[nodiscard]] bool IsPolygonValid(const Polygon& polygon) const;

    void ClearCurrentPolygon();
    void ClearAllPolygons();
    void RemoveLastVertexFromCurrentPolygon();
    [[nodiscard]] bool HasCurrentPolygon() const;
    [[nodiscard]] const std::vector<QPointF>& GetCurrentPolygonVertices() const;

    void UpdateDimensions(int width, int height);
    [[nodiscard]] int GetWidth() const;
    [[nodiscard]] int GetHeight() const;

   private:
    std::vector<Polygon> polygons_;
    std::vector<QPointF> current_polygon_vertices_;
    QPointF light_source_;
    std::vector<QPointF> multiple_light_sources_;
    std::vector<QPointF> static_lights_;
    int width_;
    int height_;
    double circle_radius_;

    void AddBoundaryPolygon();
    static bool LineSegmentsIntersect(
        const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4);
};

#endif
