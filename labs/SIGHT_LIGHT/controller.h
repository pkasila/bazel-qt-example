#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <QPointF>
#include "polygon.h"
#include "ray.h"

class Controller
{
public:
    Controller(int width = 800, int height = 600);

    const std::vector<Polygon>& getPolygons() const;
    void addPolygon(const Polygon& polygon);
    void addVertexToLastPolygon(const QPointF& new_vertex);
    void updateLastPolygon(const QPointF& new_vertex);

    QPointF getLightSource() const;
    std::vector<QPointF> getLightSources();
    void setLightSource(const QPointF& light_source);

    Polygon calculateLightArea() const;
    Polygon calculateLightAreaForSource(const QPointF& source) const;

    Polygon createLightArea() const;
    std::vector<Polygon> createLightAreas();
    void GenerateShades();

    void setLightSources(const std::vector<QPointF>& sources);

    std::vector<Polygon> calculateAllLightAreas();

private:
    std::vector<Polygon> m_polygons;
    QPointF m_light_source ;
    Polygon m_bounding_polygon;
    int m_width;
    int m_height;

   std::optional<QPointF> lineIntersection(const QPointF& p1, const QPointF& p2,
                                          const QPointF& p3, const QPointF& p4) const;
    bool pointOnSegment(const QPointF& p, const QPointF& a, const QPointF& b) const;
    void initializeBoundingPolygon();
    std::vector<Ray> castRays() const;

    std::vector<Ray> castRaysSources(const QPointF& source) const;
    void intersectRays(std::vector<Ray>* rays) const;
    void removeAdjacentRays(std::vector<Ray>* rays) const;
    static bool compareRaysByAngle(const Ray& a, const Ray& b);
    bool pointOnRay(const QPointF& p, const QPointF& ray_begin, const QPointF& ray_end) const;

    std::vector<QPointF> m_light_sources;
     std::vector<QPointF> light_sources_shades;


};

#endif // CONTROLLER_H
