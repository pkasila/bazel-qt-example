#include "controller.h"
#include <cmath>
#include <QDebug>

Controller::Controller(const QPointF source, int width, int height) : light_source(source) {
    polygons.push_back(Polygon({QPointF(0, 0), QPointF(0, height), QPointF(width, 0), QPointF(width, height)}));
}
Controller::Controller(const QPointF source, QColor color){
    light_source = source;
    this->color = color;
}

std::vector<Polygon>& Controller::GetPolygons(){
    return polygons;
}

void Controller::AddPolygon(Polygon& new_polygon){
    polygons.push_back(new_polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex){
    if(!polygons.empty()){
        polygons.back().UpdateLastVertex(new_vertex);
    }
}

void Controller::SetSource(const QPointF source){
    light_source = source;
}

QPointF Controller::GetSource() const{
    return light_source;
}

std::vector<Ray> Controller::CastRays(){
    std::vector<Ray> rays;
    for(auto& polygon : polygons){
        for(auto& vertex : polygon.GetVertices()){
            double alpha = std::atan2((vertex.y() - light_source.y()), (vertex.x() - light_source.x()));
            Ray ray(light_source, vertex, alpha);
            /*for(double i = 0.01; i <= 0.1; i += 0.077){
                rays.push_back(ray.Rotate(i));
                rays.push_back(ray.Rotate(-i));
            }*/
            rays.push_back(ray);
            rays.push_back(ray.Rotate(0.01));
            rays.push_back(ray.Rotate(-0.01));

        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>& rays){
    for(auto& polygon : polygons){
        for(auto& ray : rays){
            auto intersection = polygon.IntersectRay(ray);
            if(intersection != std::nullopt){
                int intersection_len = std::hypot(intersection->y() - ray.GetBegin().y(), intersection->x() - ray.GetBegin().x());
                int len = std::hypot(ray.GetEnd().y() - ray.GetBegin().y(), ray.GetEnd().x() - ray.GetBegin().x());
                if(intersection_len < len - 1e-6){
                    ray.SetEnd(intersection.value());
                }
            }
        }
    }
}

Polygon Controller::CreateLightArea(){
    std::vector<Ray> rays = CastRays();
    IntersectRays(rays);
    std::stable_sort(rays.begin(), rays.end(), [](Ray a, Ray b){
        return a.GetAngle() < b.GetAngle();
    });

    std::vector<QPointF> vertices;
    for(const auto& ray : rays){
        vertices.push_back(ray.GetEnd());
    }
    return Polygon(vertices);
}

QColor Controller::GetColor(){
    return color;
}

void Controller::SetSize(int width, int height){
    if(polygons.empty()){
        polygons.push_back(Polygon({QPointF(0, 0), QPointF(0, height), QPointF(width, height), QPointF(width, 0)}));
    } else{
        polygons.front() = Polygon({QPointF(0, 0), QPointF(0, height), QPointF(width, height), QPointF(width, 0)});
    }
}

