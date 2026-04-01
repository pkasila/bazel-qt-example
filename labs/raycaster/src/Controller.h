#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Polygon.h"
#include "Ray.h"

#include <QColor>
#include <QPointF>
#include <cmath>
#include <vector>

class Controller {
   public:
    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon&, PolygonType type = PolygonType::Finished);
    void RemoveLastPolygon();
    void AddVertexToLastPolygon(const QPointF&);
    void UpdateLastPolygonVertex(const QPointF&);
    void SetLastPolygonType(PolygonType type);
    [[nodiscard]] QPointF GetLightSource() const;
    void SetLightSource(const QPointF&);
    [[nodiscard]] std::vector<Ray> CastRays(const QPointF&) const;
    void IntersectRays(std::vector<Ray>* rays) const;
    static void RemoveAdjacentRays(std::vector<Ray>* rays);
    [[nodiscard]] Polygon CreateLightArea(const QPointF&) const;
    void Scale(const QPointF&);
    [[nodiscard]] bool IsTooClose(const QPointF&, const double& k_max_dist = 1e-9) const;
    [[nodiscard]] const double& GetLightRadius() const;
    [[nodiscard]] const std::vector<QPointF>& GetDeltaLights() const;
    [[nodiscard]] const std::vector<std::tuple<QPointF, QColor, int>>& GetStaticLights() const;
    [[nodiscard]] bool StaticLightsOverflow() const;
    void AddStaticLight(const QPointF&, const QColor& color = Qt::white, const int& radius = 1);
    [[nodiscard]] bool CanPlaceLight(const QPointF&) const;
    void RemoveLastStaticLight();
    [[nodiscard]] bool CanAddLastPolygonVertex(const QPointF&) const;

   private:
    std::vector<Polygon> polygons_{};
    QPointF light_source_{};
    const double kLightRadius = 2;
    const double kOrbitRadius = 10;
    const double kNumSatellites = 9;
    const std::vector<QPointF> kDeltaLights = [&] {
        std::vector<QPointF> deltas;
        deltas.reserve(kNumSatellites + 1);
        deltas.emplace_back(0, 0);
        for (int i = 0; i < kNumSatellites; ++i) {
            const double angle = (2 * M_PI * i) / kNumSatellites;
            deltas.emplace_back(kOrbitRadius * std::cos(angle), kOrbitRadius * std::sin(angle));
        }
        return deltas;
    }();

    const size_t kMaxStaticLights = 20;
    std::vector<std::tuple<QPointF, QColor, int>> static_lights_{};
};

#endif