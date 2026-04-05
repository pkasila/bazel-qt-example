#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "math.h"

#include <algorithm>
#include <vector>

namespace raycaster {

class Controller {
   public:
    Controller() : active_light_(100, 100) {
    }

    // Полигоны
    const std::vector<Polygon>& GetPolygons() const {
        return polygons_;
    }

    void StartNewPolygon(const QPointF& p) {
        polygons_.emplace_back(std::vector<QPointF>{p});
    }

    void AddVertexToLastPolygon(const QPointF& p) {
        if (!polygons_.empty()) {
            polygons_.back().AddVertex(p);
        }
    }

    void UpdateLastPolygon(const QPointF& p) {
        if (!polygons_.empty()) {
            polygons_.back().UpdateLastVertex(p);
        }
    }

    void ClearPolygons() {
        polygons_.clear();
    }

    // Свет
    void SetActiveLight(const QPointF& pos) {
        active_light_ = pos;
    }

    QPointF GetActiveLight() const {
        return active_light_;
    }

    void AddStationaryLight(const QPointF& pos) {
        stationary_lights_.push_back(pos);
    }

    const std::vector<QPointF>& GetStationaryLights() const {
        return stationary_lights_;
    }

    void ClearLights() {
        stationary_lights_.clear();
    }

    void SetBounds(double w, double h) {
        bounds_polygon_ = Polygon({{0, 0}, {w, 0}, {w, h}, {0, h}});
    }

    // Главный метод генерации области для любой точки
    Polygon CreateLightArea(const QPointF& origin) const;

    std::vector<QPointF> GetActiveLightCluster() const {
        std::vector<QPointF> cluster;
        cluster.push_back(active_light_);  // Центральный источник

        double radius = 15.0;              // Радиус "лампочки". Чем больше, тем мягче тени.
        int num_surrounding = 8;

        for (int i = 0; i < num_surrounding; ++i) {
            double angle = (2.0 * M_PI * i) / num_surrounding;
            cluster.push_back(
                active_light_ + QPointF(std::cos(angle) * radius, std::sin(angle) * radius));
        }
        return cluster;
    }

   private:
    std::vector<Polygon> polygons_;
    Polygon bounds_polygon_;
    std::vector<QPointF> stationary_lights_;
    QPointF active_light_;
};

}  // namespace raycaster

#endif