#ifndef Controller_H
#define Controller_H

#include "polygon.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <QVector>
#include <QPointF>
#include <QPainter>

class Controller{
public:
    Controller();
    const QVector<Polygon>& GetPolygons();
    void AddPolygon(const Polygon& p);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);
    QPointF getLightSource();
    QPointF getLightSource2();
    void setLightSource(QPointF point);
    void setLightSource2(QPointF point);
    Polygon getLastPolygon();
    bool isEmpty();
    void clear();
    size_t size();
    QVector<QPointF> getPolygon(size_t n);
    std::vector<Ray> CastRays(QPointF ls);
    void IntersectRays(std::vector<Ray>* rays);
    void RemoveAdjacentRays(std::vector<Ray>* rays);
    Polygon CreateLightArea(QPointF ls);
    void createOffsetRay(std::vector<Ray>& rays, const Polygon& polygon,
                                    const QPointF& vertex, double base_angle, double angle_offset);
    void DrawLightArea(QPainter& painter, size_t n);
    void setvls (QPointF p, size_t n);
    QPointF getvls (size_t n);

private:
    QVector<Polygon> polygons;
    std::vector<QPointF> light_sources;
    QPointF light_source_;
    QPointF light_source_2;
};

#endif // Controller_H
