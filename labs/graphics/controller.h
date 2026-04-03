#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"
#include "lightsource.h"
#include <vector>
#include <QColor>

class Controller {
public:
    enum class Mode {Light, Polygons, Lamping};
    Controller();

    /// polygons
    [[nodiscard]] const std::vector<Polygon>& GetPolygons();
    void AddPolygon(const Polygon&);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);
    void DeletePolygon();
    Polygon CreateLightArea(std::vector<Ray>* rays);
    void SetPolygonVisibility(bool new_status);
    bool IsPolygonVisible() const;
    QColor PolygonBorderColor() const;

    /// light source
    void AddLightSource(const LightSource& new_light_source);
    void DeleteLightSource();
    LightSource& MainLight();
    LightSource& GetLight(int index);
    void SetLightSourceVisibility(bool new_state);
    bool IsLightSourceVisible() const;
    void SetMainLightSourceVisibility(bool new_state);
    bool IsMainLightSourceVisible() const;
    void SetMode(Mode new_mode);
    [[nodiscard]]Mode GetMode() const;
    void SetDrawing(bool new_state);
    bool IsDrawing() const;
    int LightsNumber() const;
    int LampLightsNumber() const;
    double LampRadius() const;
    QColor BackGroundColor() const;
    QColor LightSourceColor() const;

    /// rays
    std::vector<Ray> CastRays(QPointF light_source_position);
    void IntersectRays(std::vector<Ray>* rays);
    void SortRaysByAngle(std::vector<Ray>* rays);
    void RemoveAdjacentRays(std::vector<Ray>* rays);
    Polygon CreateLightArea();
private:
    Mode mode_ = Mode::Light;
    std::vector<LightSource> lights_;
    std::vector<Polygon> polygons_;
    QPointF light_source_;
    bool light_source_visibility_ = true;
    bool main_light_source_visibility_ = true;
    bool is_drawing_polygons = false;
    bool polygons_visibility_ = true;
    QColor bg_color_ = QColor(0, 0, 10);
    QColor light_source_color_ = QColor(135,206,250, 100);
    QColor polygon_border_color_ = QColor(0, 76, 153);
};

#endif  // CONTROLLER_H
