#pragma once

#include "Ray.h"

#include <QPointF>
#include <optional>
#include <vector>

class Polygon {
   public:
    explicit Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {
    }

    [[nodiscard]] const auto& GetVertices() const {
        return vertices_;
    }

    auto& GetVertices()  {
        return vertices_;
    }

    void AddVertex(const QPointF& vertex) {
        vertices_.push_back(vertex);
    }

    void UpdateLastVertex(const QPointF& vertex) {
        vertices_.back() = vertex;
    }

    [[nodiscard]] std::optional<QPointF> IntersectRay(const Ray& ray) const;

   private:
    std::vector<QPointF> vertices_;
};
