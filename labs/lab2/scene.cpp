#include "scene.h"

#include <algorithm>
#include <cmath>

void Scene::add_obstacle(Obstacle obs) {
  obstacles_.push_back(std::move(obs));
}

void Scene::append_to_last(QPointF p) {
  if (!obstacles_.empty()) obstacles_.back().push_point(p);
}

void Scene::update_last(QPointF p) {
  if (!obstacles_.empty()) obstacles_.back().replace_last(p);
}

void Scene::set_wall(Obstacle wall) {
  if (obstacles_.empty()) {
    obstacles_.insert(obstacles_.begin(), std::move(wall));
  } else {
    obstacles_.front() = std::move(wall);
  }
}

std::vector<LightRay> Scene::emit_rays() const {
  std::vector<LightRay> result;
  constexpr double kFar = 1e4;
  constexpr double kEps = 0.0001;

  for (auto& obs : obstacles_) {
    for (auto& pt : obs.points()) {
      double a = std::atan2(pt.y() - lamp_.y(), pt.x() - lamp_.x());
      QPointF far_pt(lamp_.x() + kFar * std::cos(a),
                     lamp_.y() + kFar * std::sin(a));
      LightRay base(lamp_, far_pt, a);
      result.push_back(base.rotated(-kEps));
      result.push_back(base);
      result.push_back(base.rotated(kEps));
    }
  }
  return result;
}

void Scene::clip_rays(std::vector<LightRay>& rays) const {
  for (auto& r : rays) {
    for (auto& obs : obstacles_) {
      auto hit = obs.cast(r);
      if (!hit) continue;

      double hx = hit->x() - r.origin().x(), hy = hit->y() - r.origin().y();
      double ex = r.target().x() - r.origin().x(),
             ey = r.target().y() - r.origin().y();
      if (hx * hx + hy * hy < ex * ex + ey * ey) r.set_target(*hit);
    }
  }
}

void Scene::deduplicate(std::vector<LightRay>& rays) const {
  if (rays.size() < 2) return;

  std::vector<LightRay> kept;
  kept.reserve(rays.size());
  kept.push_back(rays.front());

  for (size_t i = 1; i < rays.size(); ++i) {
    double dx = rays[i].target().x() - kept.back().target().x();
    double dy = rays[i].target().y() - kept.back().target().y();
    if (dx * dx + dy * dy > 1.0) kept.push_back(rays[i]);
  }
  rays = std::move(kept);
}

Obstacle Scene::compute_visibility() const {
  auto rays = emit_rays();
  clip_rays(rays);

  std::sort(rays.begin(), rays.end(),
            [](const LightRay& a, const LightRay& b) {
              return a.direction() < b.direction();
            });

  deduplicate(rays);

  std::vector<QPointF> hull;
  hull.reserve(rays.size());
  for (auto& r : rays) hull.push_back(r.target());
  return Obstacle(std::move(hull));
}
