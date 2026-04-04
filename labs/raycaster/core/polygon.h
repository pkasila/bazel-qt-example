#pragma once

#include <QPointF>
#include <optional>
#include <vector>

namespace raycaster {

class Ray;

class Polygon {
   public:
    explicit Polygon(const std::vector<QPointF>& vertices = {});

    const std::vector<QPointF>& GetVertices() const;

    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);

    std::optional<QPointF> IntersectRay(const Ray& ray) const;

   private:
    std::vector<QPointF> vertices_;
};

}  // namespace raycaster
