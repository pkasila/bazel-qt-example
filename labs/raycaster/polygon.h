#ifndef POLYGON_H
#define POLYGON_H

#include "functions.h"
#include "ray.h"

#include <QPoint>
#include <optional>
#include <vector>

namespace PolygonShapeNS {

class PolygonShape {
   public:
    PolygonShape();
    explicit PolygonShape(const std::vector<QPoint>& points);
    void addVertex(const QPoint& pt);
    void updateLastVertex(const QPoint& pt);
    void clear();
    const std::vector<QPoint>& getVertices() const;
    bool isValid() const;
    std::vector<QPoint> closedVertices() const;
    std::optional<QPoint> findRayIntersection(const RaySegmentNS::RaySegment& ray) const;

   private:
    std::vector<QPoint> vertices;
};

}  // namespace PolygonShapeNS

#endif  // POLYGON_H
