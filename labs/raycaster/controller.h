#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"

#include <QPointF>
#include <vector>

class Controller {
public:
    std::vector<Polygon>& GetPolygons();
    const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& a);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);
    QPointF GetLightSource() const;
    void SetLightSource(const QPointF& new_light_source);

    std::vector<Ray> CastRays() const;
    void IntersectRays(std::vector<Ray>* rays) const;
    Polygon CreateLightArea() const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;
    static double Distance(QPointF P1, QPointF P2);

    void AddStaticLight(const QPointF& pos);
    const std::vector<QPointF>& GetStaticLights() const;
    bool IsPointInsideAnyPolygon(const QPointF& point) const;
    bool IsPositionValid(const QPointF& pos) const;

    bool IsSegmentIntersectingAnyPolygon(const QPointF& P1, const QPointF& P2, size_t exclude_polygon = -1) const {
        for (size_t i = 0; i < polygons_.size(); ++i) {
            if (i == exclude_polygon) continue;

            const auto& vertices = polygons_[i].GetVertices();
            const size_t size = vertices.size();
            for (size_t j = 0; j < size; j++) {
                const QPointF& V1 = vertices[j];
                const QPointF& V2 = vertices[(j + 1) % size];
                if (Polygon::LineIntersection(P1, P2, V1, V2)) {
                    return true;
                }
            }
        }
        return false;
    }

private:
    std::vector<Polygon> polygons_;
    QPointF light_source_ = {100, 100};
    std::vector<QPointF> static_lights_;
    static double AngleBetween(const QPointF& center, const QPointF& point);
};
extern int radius;
#endif
