#pragma once

#include "polygon.h"
#include "ray.h"

#include <QPoint>
#include <vector>

struct LightSystem {
    std::vector<QPoint> light_sources;
};

class Controller {
   public:
    Controller(const std::vector<Polygon>& polygons, const std::vector<Ray>& rays);

    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPoint& new_vertex);
    void UpdateLastPolygon(const QPoint& new_vertex);

    [[nodiscard]] const std::vector<QPoint>& GetDynamicLightSource() const;
    [[nodiscard]] std::vector<QPoint> GetLightSources() const;
    [[nodiscard]] const std::vector<LightSystem>& GetLightSystems() const;
    void AddStaticLightSource(const QPoint& center);
    void RemoveLastLightSource();
    void SetLightSource(const QPoint& center);
    void SetDynamicLightSource(const QPoint& center);

    std::vector<Ray> CastRays(const QPoint& source);
    void IntersectRays(std::vector<Ray>* rays, const QPoint& source);
    void RemoveAdjacentRays(std::vector<Ray>* rays);
    Polygon CreateLightArea(const QPoint& source);

    void ReplacePolygon(size_t index, const Polygon& polygon);

   private:
    std::vector<Polygon> polygons_;
    std::vector<Ray> rays_;
    std::vector<LightSystem> light_systems_;
};
