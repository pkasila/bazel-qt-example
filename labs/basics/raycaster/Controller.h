#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Polygon.h"

#include <QObject>
#include <QPointF>
#include <QWidget>
#include <vector>

// NOLINTNEXTLINE
class Controller : public QWidget {
    Q_OBJECT
   public:
    Controller() = default;

    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const;
    ~Controller() override = default;
    // NOLINTNEXTLINE
   public slots:
    void AddPolygon(const Polygon& polygon);

    void AddVertexToLastPolygon(const QPointF& new_vertex);

    void UpdateLastPolygon(const QPointF& new_vertex);

    [[nodiscard]] QPointF GetLight() const;

    std::vector<Polygon>& Polygons() {
        return polygons_;
    }

    void SetLight(const QPointF& light);

    void AddStaticLight(const QPointF& static_light) {
        static_lights_.push_back(static_light);
    }

    void UpdateLastStaticLight(const QPointF& static_light) {
        static_lights_.back() = static_light;
    }

    std::vector<QPointF>& StaticLights() {
        return static_lights_;
    }
    
    [[nodiscard]] std::vector<Ray> CastRays() const;

    [[nodiscard]] std::vector<std::vector<Ray>> CastShadow() const;

    void IntersectRays(std::vector<Ray>& rays) const;

    static void DeleteAdjacent(std::vector<Ray>& rays);

    [[nodiscard]] Polygon CreateArea() const;

    [[nodiscard]] std::vector<Polygon> Shadow() const;

    [[nodiscard]] std::vector<Polygon> CreateStatic() const;


   private:
    QPointF light_source_;
    std::vector<Polygon> polygons_;
    std::vector<QPointF> static_lights_;
};

#endif
