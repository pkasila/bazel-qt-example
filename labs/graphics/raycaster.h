#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <QPointF>
#include <cmath>
#include <optional>
#include <vector>

class Ray {
   public:
    Ray(const QPointF& begin, const QPointF& end, double angle);

    QPointF getBegin() const {
        return m_begin;
    }

    QPointF getEnd() const {
        return m_end;
    }

    double getAngle() const {
        return m_angle;
    }

    void setEnd(const QPointF& p) {
        m_end = p;
    }

    Ray Rotate(double angle) const;

   private:
    QPointF m_begin;
    QPointF m_end;
    double m_angle;
};

class Polygon {
   public:
    Polygon() = default;

    Polygon(const std::vector<QPointF>& vertices) : m_vertices(vertices) {
    }

    const std::vector<QPointF>& GetVertices() const {
        return m_vertices;
    }

    void AddVertex(const QPointF& vertex) {
        m_vertices.push_back(vertex);
    }

    void UpdateLastVertex(const QPointF& new_vertex) {
        if (!m_vertices.empty()) {
            m_vertices.back() = new_vertex;
        }
    }

    std::optional<QPointF> IntersectRay(const Ray& ray) const;
    bool IsPointInside(const QPointF& p) const;

   private:
    std::vector<QPointF> m_vertices;
};

#endif