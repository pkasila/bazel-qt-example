#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QPointF>
#include <vector>

#include "polygon.h"
#include "ray.h"

class Controller {
 public:
  Controller() = default;

  [[nodiscard]] const std::vector<Polygon>& GetPolygons() const;
  void AddPolygon(const Polygon& polygon);
  void SetBoundaryPolygon(const Polygon& boundary);
  void AddVertexToLastPolygon(const QPointF& new_vertex);
  void UpdateLastPolygon(const QPointF& new_vertex);

  [[nodiscard]] QPointF GetLightSource() const;
  void SetLightSource(const QPointF& light_source);

  [[nodiscard]] std::vector<Ray> CastRays() const;
  void IntersectRays(std::vector<Ray>* rays) const;
  void RemoveAdjacentRays(std::vector<Ray>* rays) const;
  [[nodiscard]] Polygon CreateLightArea() const;

 private:
  std::vector<Polygon> polygons_;
  QPointF light_source_ = {0, 0};
};

#endif
