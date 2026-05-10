#pragma once

#include "ray.h"

#include <QPoint>
#include <optional>
#include <vector>

class Polygon {
   public:
    explicit Polygon(const std::vector<QPoint>& vertices) : v_(vertices) {
    }

    [[nodiscard]] QPoint GetVertice(size_t index) const;
    [[nodiscard]] const std::vector<QPoint>& GetAllVertices() const;

    void AddVertex(const QPoint& vertex);
    void UpdateLastVertex(const QPoint& new_vertex);

    [[nodiscard]] std::optional<QPoint> IntersectRay(const Ray& ray) const;
    [[nodiscard]] QPoint GetDistanceVector(const QPoint& point) const;

   private:
    std::vector<QPoint> v_;

    void SetVertice(QPoint point, size_t index = static_cast<size_t>(-1));
    void SetAllVertices(const std::vector<QPoint>& vertices);
};
