#ifndef POLYGON_H
#define POLYGON_H
#include <vector>
#include <QPointF>
#include <optional>
#include "Ray.h"

class Polygon {
public:
    Polygon() = default;
    Polygon(const std::vector<QPointF>& v) : m_vertices(v) {}
    const std::vector<QPointF>& getVertices() const { return m_vertices; }
    void AddVertex(const QPointF& v) { m_vertices.push_back(v); }
    void UpdateLastVertex(const QPointF& v) { if(!m_vertices.empty()) m_vertices.back() = v; }
    std::optional<QPointF> IntersectRay(const Ray& ray) const;
private:
    std::vector<QPointF> m_vertices;
};
#endif