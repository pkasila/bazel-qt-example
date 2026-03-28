#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <QPoint>
#include <optional>
#include <utility>

class Ray;

class Polygon {
public:
    Polygon(const std::vector<QPoint>& vertices);

    const std::vector<QPoint>& getVertices() const { return vertices_; }
    
    void AddVertex(const QPoint& vertex);
    void UpdateLastVertex(const QPoint& new_vertex);
    std::optional<QPoint> IntersectRay(const Ray& ray) const;
    
private:
    std::vector<QPoint> vertices_;
    std::optional<std::pair<QPoint, double>> IntersectRaySegment(const QPoint& ray_start, const QPoint& ray_end,
                                                               const QPoint& seg_start, const QPoint& seg_end) const;
};

#endif // POLYGON_H
