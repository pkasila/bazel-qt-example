#include <algorithm>
#include "polygon.h"

class Controller {
public:
    std::vector<Ray> castRays();
    void intersectRays(std::vector<Ray>& rays);
    void removeAdjacentRays(std::vector<Ray>& rays);
    Polygon createLightArea(QPointF customSource);

    void addPolygon(const Polygon& p) { polygons_.push_back(p); }
    void addVertexToLastPolygon(const QPointF& v) { if(!polygons_.empty()) polygons_.back().addVertex(v); }
    void updateLastPolygon(const QPointF& v) { if(!polygons_.empty()) polygons_.back().updateLastVertex(v); }
    const std::vector<Polygon>& getPolygons() const { return polygons_; }
    
    void setLightSource(QPointF p) { lightSource_ = p; }
    QPointF getLightSource() const { return lightSource_; }

private:
    std::vector<Polygon> polygons_;
    QPointF lightSource_;
};