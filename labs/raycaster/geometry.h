#pragma once

#include <optional>
#include <vector>

#include <QPoint>
#include <QPointF>

class Ray {
public:
    Ray(const QPoint& begin, const QPoint& end, double angle);
    Ray(const QPointF& begin, const QPointF& end, double angle);

    const QPointF& GetBegin() const;
    const QPointF& GetEnd() const;
    double GetAngle() const;

    void SetBegin(const QPoint& begin);
    void SetBegin(const QPointF& begin);
    void SetEnd(const QPoint& end);
    void SetEnd(const QPointF& end);
    void SetAngle(double angle);

    Ray Rotate(double angle) const;

private:
    QPointF begin_;
    QPointF end_;
    double angle_ = 0.0;
};

class Polygon {
public:
    Polygon();
    Polygon(const std::vector<QPoint>& vertices);
    Polygon(const std::vector<QPointF>& vertices);

    const std::vector<QPointF>& GetVertices() const;

    void AddVertex(const QPoint& vertex);
    void AddVertex(const QPointF& vertex);
    void UpdateLastVertex(const QPoint& new_vertex);
    void UpdateLastVertex(const QPointF& new_vertex);

    std::optional<QPointF> IntersectRay(const Ray& ray) const;

    std::size_t Size() const;
    bool Empty() const;

private:
    std::vector<QPointF> vertices_;
};

class Controller {
public:
    Controller();

    const std::vector<Polygon>& GetPolygons() const;

    void AddPolygon(const Polygon& polygon);
    void AddPolygon(const std::vector<QPoint>& vertices);
    void AddPolygon(const std::vector<QPointF>& vertices);

    void AddVertexToLastPolygon(const QPoint& new_vertex);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPoint& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    void FinalizeLastPolygon();
    void SetBounds(double width, double height);

    QPointF GetLightSource() const;
    void SetLightSource(const QPoint& light_source);
    void SetLightSource(const QPointF& light_source);

    const std::vector<QPointF>& GetLightSources() const;
    void SetLightSources(const std::vector<QPointF>& lights);

    std::vector<Ray> CastRays() const;
    std::vector<Ray> CastRays(const QPointF& light_source) const;
    void IntersectRays(std::vector<Ray>* rays) const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;
    Polygon CreateLightArea() const;
    Polygon CreateLightArea(const QPoint& light_source) const;
    Polygon CreateLightArea(const QPointF& light_source) const;
    std::vector<Polygon> CreateLightAreas() const;

private:
    std::vector<Polygon> polygons_;
    std::vector<QPointF> light_sources_;
    std::vector<QPointF> light_offsets_;

    QPointF IntersectWithBoundary(const QPointF& source, double angle) const;
};
