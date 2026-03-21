#pragma once
#include "polygon.h"
#include "ray.h"

#include <QPointF>
#include <vector>

class Controller {
   public:
    Controller();

    const std::vector<Polygon>& GetPolygons() const {
        return polygons_;
    }

    void AddPolygon(const Polygon& poly) {
        polygons_.push_back(poly);
    }

    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    QPointF GetLightSource() const {
        return light_source_;
    }

    void SetLightSource(const QPointF& source) {
        light_source_ = source;
    }

    void UpdateBounds(double width, double height);

    bool CheckCollision(const QPointF& p, double threshold = 5.0) const;

    std::vector<Ray> CastRays();
    void IntersectRays(std::vector<Ray>* rays);
    void RemoveAdjacentRays(std::vector<Ray>* rays);

    Polygon CreateLightArea();

   private:
    std::vector<Polygon> polygons_;
    QPointF light_source_;
};