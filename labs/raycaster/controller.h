#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "functions.h"
#include "polygon.h"
#include "ray.h"

#include <QPoint>
#include <optional>
#include <vector>

enum class RenderMode { Light, Polygons };

class RaycasterController {
   public:
    RaycasterController();
    void beginPolygon(const QPoint& initPt);
    void appendVertex(const QPoint& pt);
    void updateCurrentPolygon(const QPoint& pt);
    void completePolygon();
    const std::vector<PolygonShapeNS::PolygonShape>& getPolygons() const;
    QPoint getLightPosition() const;
    void setLightPosition(const QPoint& pt);
    std::vector<RaySegmentNS::RaySegment> generateLightRays(const QPoint& srcPos) const;
    std::vector<RaySegmentNS::RaySegment> generateLightRays() const;
    void processRayIntersections(std::vector<RaySegmentNS::RaySegment>* rays) const;
    void filterDuplicateRays(std::vector<RaySegmentNS::RaySegment>* rays) const;
    std::vector<QPoint> computeLightArea() const;

   private:
    std::vector<PolygonShapeNS::PolygonShape> polygonList;
    PolygonShapeNS::PolygonShape currentPolygon;
    QPoint lightPos;
    RenderMode currentMode;
    bool constructing;
};

#endif  // CONTROLLER_H
