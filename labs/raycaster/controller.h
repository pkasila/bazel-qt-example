#pragma once
#include <QtWidgets>
#include <vector>
#include "polygon.h"

class Controller{
   public:
    std::vector<Polygon>& GetPolygons();
    void AddPolygon(const Polygon& poly);
    void AddVertexToLastPolygon(const QPointF& new_vertex);

    void UpdateLastPolygon(const QPointF& new_vertex);
    void SetLight(const QPointF& ls);
    QPointF GetLight();

    int& Mode();
    std::vector<Ray> CastRays();
    void IntersectRays(std::vector<Ray>* rays);
    void RemoveAdjacentRays(std::vector<Ray>* rays);
    Polygon CreateLightArea();

    void AddStaticLight(QPointF light);
    std::vector<QPointF>& GetStaticLights();

   private:
    int mode_ = 0; //0-light 1-poly 2-st.lights
    std::vector<Polygon> polygons_;
    QPointF light_source_;
    std::vector<QPointF> st_lights_;
};