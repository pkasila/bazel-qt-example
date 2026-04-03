#ifndef POLYGON_H
#define POLYGON_H

#include <QPointF>
#include <optional>
#include <vector>

#include "ray.h"

class Polygon {
 public:
  Polygon() = default;
  explicit Polygon(const std::vector<QPointF>& vertices);

  [[nodiscard]] const std::vector<QPointF>& GetVertices() const;

  void AddVertex(const QPointF& vertex);
  void UpdateLastVertex(const QPointF& new_vertex);

  [[nodiscard]] std::optional<QPointF> IntersectRay(const Ray& ray) const;

 private:
  [[nodiscard]] std::optional<QPointF> IntersectSegment(
      const QPointF& ray_begin, const QPointF& ray_end,
      const QPointF& seg_a, const QPointF& seg_b) const;

  std::vector<QPointF> vertices_;
};

#endif
