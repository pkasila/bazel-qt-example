#pragma once

#include <QPointF>
#include <vector>

#include "light_ray.h"
#include "obstacle.h"

class Scene {
 public:
  [[nodiscard]] const std::vector<Obstacle>& obstacles() const { return obstacles_; }

  void add_obstacle(Obstacle obs);
  void append_to_last(QPointF p);
  void update_last(QPointF p);
  void set_wall(Obstacle wall);

  [[nodiscard]] QPointF lamp() const { return lamp_; }
  void set_lamp(QPointF p) { lamp_ = p; }

  [[nodiscard]] std::vector<LightRay> emit_rays() const;
  void clip_rays(std::vector<LightRay>& rays) const;
  void deduplicate(std::vector<LightRay>& rays) const;
  [[nodiscard]] Obstacle compute_visibility() const;

 private:
  std::vector<Obstacle> obstacles_;
  QPointF lamp_ = {0, 0};
};
