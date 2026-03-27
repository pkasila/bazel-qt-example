#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../geom/polygon.h"

#include <QObject>
#include <QPointF>
#include <QWidget>
#include <vector>

enum class light_type : int8_t {
    main_light,
    static_light,
    satellite_light,
};

class Controller {
   public:
    Controller() = default;

    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const;
    [[nodiscard]] std::vector<Polygon>& GetPolygons();
    void AddPolygon(const Polygon& new_polygon);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    [[nodiscard]] QPointF GetLightSource() const;
    void SetLightSource(const QPointF& light_source);
    [[nodiscard]] const std::vector<QPointF>& GetStaticLights() const;
    [[nodiscard]] std::vector<QPointF>& GetStaticLights();
    void AddStaticLight(const QPointF& new_static_light);
    void UpdateLastStaticLight(const QPoint& new_static_light);
    void SetupSatelliteLights();

    [[nodiscard]] bool IsDragging() const;
    void SetDragging(bool dragging);
    [[nodiscard]] bool IsComplete() const;
    void SetComplete(bool complete);
    void FinishPolygon();

    [[nodiscard]] std::vector<Ray> CastRays(const QPointF& light_source) const;
    void IntersectRays(std::vector<Ray>* rays) const;
    static void RemoveAdjacentRays(std::vector<Ray>* rays);
    [[nodiscard]] std::vector<std::tuple<QPointF, Polygon, light_type>> CreateLightArea() const;

   private:
    std::vector<Polygon> polygons_;
    std::vector<QPointF> static_lights_;
    std::vector<QPointF> satellite_lights_;
    QPointF light_source_;
    bool is_dragging_ = false;
    bool is_complete_ = true;
};

#endif