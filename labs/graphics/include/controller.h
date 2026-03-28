#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "polygon.h"

#include <QPointF>
#include <vector>

class Controller {
   public:
    [[nodiscard]] std::vector<Polygon>& GetPolygons();
    [[nodiscard]] const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& a);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    bool DoesPolygonIntersectOthers(const std::vector<QPointF>& new_vertices);
    void UpdateLastPolygon(const QPointF& new_vertex);
    [[nodiscard]] QPointF GetLightSource() const;
    void SetLightSource(const QPointF& new_light_source);

    [[nodiscard]] std::vector<Ray> CastRays() const;
    void IntersectRays(std::vector<Ray>* rays) const;
    [[nodiscard]] Polygon CreateLightArea() const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;
    static double Distance(QPointF a, QPointF b);

    void AddStaticLight(const QPointF& pos);
    [[nodiscard]] const std::vector<QPointF>& GetStaticLights() const;
    bool IsPointInsideAnyPolygon(const QPointF& point) const;
    bool IsPositionValid(const QPointF& pos) const;

    bool IsSegmentIntersectingAnyPolygon(const QPointF& p1, const QPointF& p2, size_t exclude_polygon = -1) const {
        for (size_t i = 0; i < polygons_.size(); ++i) {
            if (i == exclude_polygon) continue;

            const auto& vertices = polygons_[i].GetVertices();
            const size_t n = vertices.size();
            for (size_t j = 0; j < n; ++j) {
                const QPointF& v1 = vertices[j];
                const QPointF& v2 = vertices[(j + 1) % n];

                if (Polygon::LineIntersection(p1, p2, v1, v2)) {
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
#endif  // CONTROLLER_H
