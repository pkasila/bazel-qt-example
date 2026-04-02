#pragma once

#include <QPointF>
#include <vector>

#include "Polygon.h"
#include "Ray.h"

class Controller {
public:
    const std::vector<Polygon>& GetPolygons() const;

    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);

    Polygon& GetLastPolygon();
    const Polygon& GetLastPolygon() const;

    void RemoveLastVertexFromLastPolygon();
    void RemoveLastPolygon();
    void SetPolygon(size_t index, const Polygon& polygon);

    size_t PolygonCount() const;
    bool Empty() const;

    const QPointF& GetLightSource() const;
    void SetLightSource(const QPointF& light_source);

    const std::vector<QPointF>& GetLightOffsets() const;
    void SetLightOffsets(const std::vector<QPointF>& offsets);
    std::vector<QPointF> GetAllLightSources() const;

    std::vector<Ray> CastRays(const QPointF& light_source) const;
    void IntersectRays(std::vector<Ray>* rays) const;
    void RemoveAdjacentRays(std::vector<Ray>* rays) const;
    Polygon CreateLightArea(const QPointF& light_source) const;
    bool IsLightSourcePositionValid(const QPointF& pos) const;
    bool CanPlacePolygon(const Polygon& polygon, int ignore_index = -1, bool treat_as_open = false) const;

    const std::vector<QPointF>& GetStaticLightSources() const;
    void AddStaticLightSource(const QPointF& light_source);
    void ClearStaticLightSources();

    std::vector<QPointF> GetAllStaticLightPoints() const;

private:
    std::vector<Polygon> polygons_;
    QPointF light_source_;
    std::vector<QPointF> light_offsets_;
    std::vector<QPointF> static_light_sources_;
};