#include "polygon.h"

#include <cmath>
#include <limits>

Polygon::Polygon(const std::vector<QPointF>& vertices)
    : vertices_(vertices) {
}

const std::vector<QPointF>& Polygon::GetVertices() const {
  return vertices_;
}

void Polygon::AddVertex(const QPointF& vertex) {
  vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
  if (!vertices_.empty()) {
    vertices_.back() = new_vertex;
  }
}

std::optional<QPointF> Polygon::IntersectSegment(
    const QPointF& ray_begin, const QPointF& ray_end,
    const QPointF& seg_a, const QPointF& seg_b) const {
  double r_dx = ray_end.x() - ray_begin.x();
  double r_dy = ray_end.y() - ray_begin.y();
  double s_dx = seg_b.x() - seg_a.x();
  double s_dy = seg_b.y() - seg_a.y();

  double denom = r_dx * s_dy - r_dy * s_dx;
  if (std::abs(denom) < 1e-10) {
    return std::nullopt;
  }

  double t = ((seg_a.x() - ray_begin.x()) * s_dy -
              (seg_a.y() - ray_begin.y()) * s_dx) /
             denom;
  double u = ((seg_a.x() - ray_begin.x()) * r_dy -
              (seg_a.y() - ray_begin.y()) * r_dx) /
             denom;

  if (t >= 0 && u >= 0 && u <= 1.0) {
    double ix = ray_begin.x() + t * r_dx;
    double iy = ray_begin.y() + t * r_dy;
    return QPointF(ix, iy);
  }

  return std::nullopt;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
  if (vertices_.size() < 2) {
    return std::nullopt;
  }

  std::optional<QPointF> closest;
  double min_dist = std::numeric_limits<double>::max();

  QPointF begin = ray.GetBegin();
  QPointF end = ray.GetEnd();

  for (size_t i = 0; i < vertices_.size(); ++i) {
    size_t j = (i + 1) % vertices_.size();
    auto hit = IntersectSegment(begin, end, vertices_[i], vertices_[j]);
    if (hit.has_value()) {
      double dx = hit->x() - begin.x();
      double dy = hit->y() - begin.y();
      double dist = dx * dx + dy * dy;
      if (dist < min_dist) {
        min_dist = dist;
        closest = hit;
      }
    }
  }

  return closest;
}
