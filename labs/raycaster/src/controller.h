//
// Created by blalex on 4/8/25.
//

#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "polygon.h"
#include <QSize>

class Controller {
   public:
    Controller();

    const std::vector<Polygon>& GetPolygons();
    void AddPolygon(const Polygon&);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);
    [[nodiscard]] QPointF GetLightSource() const;
    void SetLightSource(const QPointF& light_source);
    [[nodiscard]] int GetMode() const;
    void SetMode(const int mode);

    std::vector<Ray> CastRays();

    void IntersectRays(std::vector<Ray> &rays);

    void RemoveAdjacentRays(std::vector<Ray> &rays);

    Polygon CreateLightArea();

    void ChangeMode(int mode);

    void UpdateBorderRect(const QSize& size);

   private:
    std::vector<Polygon> polygons_{};
    QPointF light_source_{};
    Polygon light_area_{};

    int mode_{};
};

#endif  // CONTROLLER_H
