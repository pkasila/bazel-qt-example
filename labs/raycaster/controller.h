#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"
#include "ray.h"

#include <vector>
#include <QPaintEvent>
#include <QWidget>

class Controller : public QWidget
{
public:
    Controller();
    const std::vector<Polygon>& GetPolygons();
    void AddPolygon(const Polygon&);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    void paintEvent(QPaintEvent*) override;
    void SetMode(int index);
    std::vector<Ray> CastRays();
    void IntersectRays(std::vector<Ray>& rays);
    Polygon CreateLightArea();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    std::vector<Polygon> polygons;
    std::vector<QPointF> light_sources;
    QPointF light_source;
    int mode;
};

#endif // CONTROLLER_H
