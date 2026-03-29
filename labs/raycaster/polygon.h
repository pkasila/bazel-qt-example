#pragma once
#include <QtWidgets>
#include <cmath>
#include <cstddef>
#include <optional>
#include <vector>
#include "ray.h"

class Polygon{
   public:
    explicit Polygon(const std::vector<QPointF>& vertices);
    std::vector<QPointF> Get();
    void AddVertex(QPointF vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    std::optional<QPointF> IntersectRay(const Ray& ray);
   private:
    std::vector<QPointF> vertices_;    
};