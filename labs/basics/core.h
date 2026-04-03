#ifndef CORE_H
#define CORE_H

#include <QPointF>
#include <QColor>
#include <vector>
#include <optional>

inline double CrossProduct(const QPointF& v1, const QPointF& v2) {
    return v1.x() * v2.y() - v1.y() * v2.x();
}

inline QPointF Subtract(const QPointF& p1, const QPointF& p2) {
    return QPointF(p1.x() - p2.x(), p1.y() - p2.y());
}

inline double Distance(const QPointF& p1, const QPointF& p2) {
    return std::hypot(p1.x() - p2.x(), p1.y() - p2.y());
}

struct Light {
    QPointF pos;
    QColor color = Qt::white;
};

// ================= CLASSS RAY =================
class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle);
    QPointF getBegin() const;
    void setBegin(const QPointF& begin);
    QPointF getEnd() const;
    void setEnd(const QPointF& end);
    double getAngle() const;
    void setAngle(double angle);
    Ray Rotate(double angleOffset) const;

private:
    QPointF m_begin;
    QPointF m_end;
    double m_angle;
};

// ================= CLASS POLYGON =================
class Polygon {
public:
    Polygon(const std::vector<QPointF>& vertices = {});
    const std::vector<QPointF>& GetVertices() const;
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPointF& new_vertex);
    std::optional<QPointF> IntersectRay(const Ray& ray) const;

    bool ContainsPoint(const QPointF& pt) const;
    void MakeClockwise();

private:
    std::vector<QPointF> m_vertices;
};

// ================= CLASS CONTROLLER =================
class Controller {
public:
    const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& p);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);
    void FinalizeLastPolygon();

    const std::vector<Light>& GetLights() const;
    void AddLight(const QPointF& pos, const QColor& color = Qt::white);
    void RemoveLight(size_t index);
    void SetLightPos(size_t index, const QPointF& pos);
    void SetLightColor(size_t index, const QColor& color);

    std::vector<Ray> CastRays(const QPointF& light_source) const;
    void IntersectRays(std::vector<Ray>* rays) const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;
    Polygon CreateLightArea(const QPointF& light_source) const;

    bool IsPointInAnyPolygon(const QPointF& pt) const; // Праверка калізій

private:
    std::vector<Polygon> m_polygons;
    std::vector<Light> m_lights;
};

#endif // CORE_H