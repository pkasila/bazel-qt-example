#pragma once

#include <QPointF>
#include <vector>
#include <optional>
#include <cmath>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle);

    QPointF getBegin() const;
    void setBegin(const QPointF& begin);

    QPointF getEnd() const;
    void setEnd(const QPointF& end);

    double getAngle() const;
    void setAngle(const double angle);

    Ray Rotate(double angle_rad) const;

private:
    QPointF m_begin;
    QPointF m_end;
    double m_angle;
};

class Polygon {
public:
    explicit Polygon(const std::vector<QPointF>& vertices);

    const std::vector<QPointF>& getVertices() const;

    void AddVertex(const QPointF& vertex);

    void UpdateLastVertex(const QPointF& new_vertex);

    std::optional<QPointF> IntersectRay(const Ray& ray) const;

    void RemoveLastVertex();

private:
    std::vector<QPointF> m_vertices;
};