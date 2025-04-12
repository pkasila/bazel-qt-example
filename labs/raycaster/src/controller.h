#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "ray.h"
#include "polygon.h"
#include <QColor>

class Controller
{
public:
    Controller() = default;
    Controller(const QPointF source, QColor color);
    Controller(const QPointF source, int width, int height);

    std::vector<Polygon>& GetPolygons();

    void AddPolygon(Polygon& new_polygon);

    void AddVertexToLastPolygon(const QPointF& new_vertex);

    void SetSource(const QPointF source);

    void SetSize(int width, int height);

    QPointF GetSource() const;

    std::vector<Ray> CastRays();

    void IntersectRays(std::vector<Ray>& rays);

    Polygon CreateLightArea();

    QColor GetColor();

private:
    QPointF light_source;
    std::vector<Polygon> polygons;
    QColor color;

};

#endif // CONTROLLER_H
