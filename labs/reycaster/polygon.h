#ifndef POLYGON_H
#define POLYGON_H

#include <QPointF>
#include <optional>
#include <vector>

class Ray;

class Polygon {
public:
    explicit Polygon(const std::vector<QPointF>& vertices = {});
    
    // Геттер для вершин
    const std::vector<QPointF>& getVertices() const { return vertices_; }
    
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    
    std::optional<QPointF> IntersectRay(const Ray& ray) const;
    
private:
    std::vector<QPointF> vertices_;
    
    // Вспомогательные методы
    std::optional<QPointF> IntersectSegment(const QPointF& p1, const QPointF& p2, 
                                           const Ray& ray) const;
    double Distance(const QPointF& p1, const QPointF& p2) const;
};

#endif // POLYGON_H
