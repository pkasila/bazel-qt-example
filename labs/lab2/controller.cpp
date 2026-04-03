#include "controller.h"

#include <algorithm>
#include <cmath>

const std::vector<Polygon>& Controller::GetPolygons() const {
  return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
  polygons_.push_back(polygon);
}

void Controller::SetBoundaryPolygon(const Polygon& boundary) {
  if (polygons_.empty()) {
    polygons_.insert(polygons_.begin(), boundary);
  } else {
    polygons_[0] = boundary;
  }
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
  if (!polygons_.empty()) {
    polygons_.back().AddVertex(new_vertex);
  }
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
  if (!polygons_.empty()) {
    polygons_.back().UpdateLastVertex(new_vertex);
  }
}

QPointF Controller::GetLightSource() const {
  return light_source_;
}

void Controller::SetLightSource(const QPointF& light_source) {
  light_source_ = light_source;
}

std::vector<Ray> Controller::CastRays() const {
  std::vector<Ray> rays;
  constexpr double kMaxDist = 10000.0;

  for (const auto& polygon : polygons_) {
    for (const auto& vertex : polygon.GetVertices()) {
      double dx = vertex.x() - light_source_.x();
      double dy = vertex.y() - light_source_.y();
      double angle = std::atan2(dy, dx);

      QPointF far_end(light_source_.x() + kMaxDist * std::cos(angle),
                      light_source_.y() + kMaxDist * std::sin(angle));
      Ray base_ray(light_source_, far_end, angle);
      rays.push_back(base_ray);
      rays.push_back(base_ray.Rotate(-0.0001));
      rays.push_back(base_ray.Rotate(0.0001));
    }
  }

  return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
  for (auto& ray : *rays) {
    for (const auto& polygon : polygons_) {
      auto hit = polygon.IntersectRay(ray);
      if (hit.has_value()) {
        double dx_hit = hit->x() - ray.GetBegin().x();
        double dy_hit = hit->y() - ray.GetBegin().y();
        double dist_hit = dx_hit * dx_hit + dy_hit * dy_hit;

        double dx_end = ray.GetEnd().x() - ray.GetBegin().x();
        double dy_end = ray.GetEnd().y() - ray.GetBegin().y();
        double dist_end = dx_end * dx_end + dy_end * dy_end;

        if (dist_hit < dist_end) {
          ray.SetEnd(*hit);
        }
      }
    }
  }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
  if (rays->empty()) {
    return;
  }

  constexpr double kMinDist = 1.0;

  std::vector<Ray> result;
  result.push_back((*rays)[0]);

  for (size_t i = 1; i < rays->size(); ++i) {
    const QPointF& prev_end = result.back().GetEnd();
    const QPointF& cur_end = (*rays)[i].GetEnd();
    double dx = cur_end.x() - prev_end.x();
    double dy = cur_end.y() - prev_end.y();
    double dist = dx * dx + dy * dy;

    if (dist > kMinDist * kMinDist) {
      result.push_back((*rays)[i]);
    }
  }

  *rays = std::move(result);
}

Polygon Controller::CreateLightArea() const {
  std::vector<Ray> rays = CastRays();
  IntersectRays(&rays);

  std::sort(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) {
    return a.GetAngle() < b.GetAngle();
  });

  RemoveAdjacentRays(&rays);

  std::vector<QPointF> vertices;
  vertices.reserve(rays.size());
  for (const auto& ray : rays) {
    vertices.push_back(ray.GetEnd());
  }

  return Polygon(vertices);
}
