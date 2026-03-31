#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <QPointF>
#include "polygon.h"
#include "ray.h"
#include <optional>

class Controller
{
public:
    Controller(int width = 800, int height = 600);

    const std::vector<Polygon>& getPolygons() const;
    void addPolygon(const Polygon& polygon);
    void addVertexToLastPolygon(const QPointF& new_vertex);
    void updateLastPolygon(const QPointF& new_vertex);

    QPointF getLightSource() const;
    void setLightSource(const QPointF& light_source);

    std::vector<QPointF> getLightSources() const;
    void setLightSources(const std::vector<QPointF>& sources);

    std::vector<Ray> castRays() const;
    void intersectRays(std::vector<Ray>* rays) const;
    void removeAdjacentRays(std::vector<Ray>* rays) const;
    Polygon createLightArea() const;

    Polygon calculateLightAreaForSource(const QPointF& source) const;
    std::vector<Polygon> calculateAllLightAreasForMultipleSources();

     const std::vector<QPointF>& getBoundingBoxVertices() const;


private:
    std::vector<Polygon> m_scene_polygons;
    Polygon m_bounding_box_polygon;
    int m_scene_width;
    int m_scene_height;

    QPointF m_single_light_source_pos;
    std::vector<QPointF> m_multiple_light_sources_pos;

    void initializeBoundingBox(int width, int height);
    std::vector<Ray> castRaysFromSource(const QPointF& source_pos) const;

    static bool compareRaysByAngle(const Ray& a, const Ray& b);
    static double pointDistanceSquared(const QPointF& p1, const QPointF& p2);
};

#endif // CONTROLLER_H

