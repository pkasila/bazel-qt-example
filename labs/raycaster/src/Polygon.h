#ifndef POLYGON_H
#define POLYGON_H

#include "Ray.h"

#include <QPointF>
#include <cstddef>
#include <optional>
#include <vector>

enum class PolygonType { Creating, Finished };

class Polygon {
   public:
    Polygon() = default;
    explicit Polygon(const std::vector<QPointF>&);
    [[nodiscard]] const std::vector<QPointF>& GetVertices() const;
    [[nodiscard]] const QPointF& GetVertex(const size_t&) const;
    [[nodiscard]] PolygonType GetType() const;
    void SetType(PolygonType type);
    void AddVertex(const QPointF&);
    void UpdateLastVertex(const QPointF&);
    [[nodiscard]] std::optional<QPointF> IntersectRay(const Ray&) const;
    void Scale(const QPointF&);
    [[nodiscard]] double GetDistance(const QPointF&) const;
    [[nodiscard]] bool ContainsPoint(const QPointF&) const;

   private:
    std::vector<QPointF> vertices_{};

    PolygonType type_ = PolygonType::Finished;

    static std::optional<QPointF> FindIntersection(
        const QPointF&, const QPointF&, const QPointF&, const QPointF&);
};

#endif  // POLYGON_H
