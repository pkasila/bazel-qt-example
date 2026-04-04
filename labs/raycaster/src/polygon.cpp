#include "polygon.h"
#include <cmath>
#include <climits>

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices(vertices){
}

std::vector<QPointF> Polygon::GetVertices() const{
    return vertices;
}

void Polygon::AddVertex(const QPointF& vertex){
    vertices.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex){
    if(!vertices.empty()){
        vertices.pop_back();
    }
    vertices.push_back(new_vertex);
}

void Polygon::Complete(){
    completed = true;
}

bool Polygon::IsComplete() const{
    return completed;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray){
    std::optional<QPointF> result;
    double minlen = std::numeric_limits<double>::max();
    for(int i = 0; i != vertices.size(); ++i){
        QPointF p1 = vertices[i];
        QPointF p2 = vertices[(i + 1) % vertices.size()];

        auto intersection = IntersectLine(p1, p2, ray);
        if(intersection != std::nullopt){
            double len = std::hypot(intersection->y() - ray.GetBegin().y(), intersection->x() - ray.GetBegin().x());
            if(len < minlen){
                minlen = len;
                result = intersection;
            }
        }
    }
    return result;

}

