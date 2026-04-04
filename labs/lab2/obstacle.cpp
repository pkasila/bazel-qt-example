#include "obstacle.h"

#include <cmath>
#include <limits>

Obstacle::Obstacle(std::vector<QPointF> pts) : pts_(std::move(pts)) {}

void Obstacle::push_point(QPointF p) { pts_.push_back(p); }

void Obstacle::replace_last(QPointF p) {
  if (!pts_.empty()) pts_.back() = p;
}

std::optional<QPointF> Obstacle::seg_intersect(QPointF r0, QPointF r1,
                                               QPointF s0, QPointF s1) {
  double rdx = r1.x() - r0.x(), rdy = r1.y() - r0.y();
  double sdx = s1.x() - s0.x(), sdy = s1.y() - s0.y();
  double cross = rdx * sdy - rdy * sdx;
  if (std::abs(cross) < 1e-9) return std::nullopt;

  double t = ((s0.x() - r0.x()) * sdy - (s0.y() - r0.y()) * sdx) / cross;
  double u = ((s0.x() - r0.x()) * rdy - (s0.y() - r0.y()) * rdx) / cross;

  if (t < 0 || u < 0 || u > 1.0) return std::nullopt;
  return QPointF(r0.x() + t * rdx, r0.y() + t * rdy);
}

std::optional<QPointF> Obstacle::cast(const LightRay& ray) const {
  if (pts_.size() < 2) return std::nullopt;

  double best_sq = std::numeric_limits<double>::max();
  std::optional<QPointF> best;

  for (size_t i = 0; i < pts_.size(); ++i) {
    auto hit = seg_intersect(ray.origin(), ray.target(),
                             pts_[i], pts_[(i + 1) % pts_.size()]);
    if (!hit) continue;

    double dx = hit->x() - ray.origin().x();
    double dy = hit->y() - ray.origin().y();
    double sq = dx * dx + dy * dy;
    if (sq < best_sq) {
      best_sq = sq;
      best = hit;
    }
  }
  return best;
}
