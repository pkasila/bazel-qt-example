//
// Created by Zazerg on 3/31/25.
//

#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "LightSource.h"
#include "Polygon.h"


class Controller {
public:
    const std::vector<Polygon>& getPolygons () const;
    void addPolygon (const Polygon& polygon);
    void addPolygon ();
    void addVertexToLastPolygon (const QPointF&);
    void updateLastPolygon (const QPointF&);
    const LightSource& getLightSource();
    void setLightSource(QPointF point, int w, int h);


    std::vector<Ray> castRays (QPointF lightSource, int w, int h);
    void intersectRays(std::vector<Ray>&, int w, int h);

    std::vector<Polygon> createHalfShades(QPointF lightSource, int w, int h);

    Polygon fromLightSource (QPointF lightSource, int w, int h);


    std::vector<LightSource>& getLightSources();


void addLightSource(LightSource);

private:

    std::vector<Polygon> polygons_ = {};
    std::vector<QPointF> ints_ = {};
    std::vector<QPointF> c_ints_ = {};
    std::vector<LightSource> lights_ = {};
};



#endif //CONTROLLER_H
