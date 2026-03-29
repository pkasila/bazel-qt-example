#include "polygon.h"
#include "ray.h"

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices){
}

std::vector<QPointF> Polygon::Get(){
    return vertices_;
}

void Polygon::AddVertex(const QPointF vertex){
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex){
    if (!vertices_.empty()){
        vertices_.back() = new_vertex;
    } else { 
        vertices_.emplace_back(new_vertex);
    }
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray){
    double mn=9999999;
    std::pair<double, double> raydel = {cos(ray.GetAngle()),sin(ray.GetAngle())};
    for (int i=0; i<vertices_.size(); i++){
        QPointF b = vertices_[i];
        QPointF e = vertices_[(i+1)%vertices_.size()];            
        std::pair<double, double> segdel = {e.x()-b.x(),e.y()-b.y()};
        if (abs(segdel.first*raydel.second - segdel.second*raydel.first) < kEps) {
            continue;
        }
        double t2 = (raydel.first*(b.y()-ray.GetBegin().y()) + raydel.second*(ray.GetBegin().x()-b.x()))/(segdel.first*raydel.second - segdel.second*raydel.first);
        double t1=0;
        if (abs(raydel.first)<kEps){
            t1 = (b.y()+segdel.second*t2-ray.GetBegin().y())/raydel.second;
        }else{
            t1 = (b.x()+segdel.first*t2-ray.GetBegin().x())/raydel.first;
        }
        if (t1>-kEps && -kEps<t2 && t2<1+kEps) {
            mn=std::min(mn,t1);
        }
    }
    if (abs(mn-9999999)<kEps){
        return std::nullopt;
    }
    QPointF ans={ray.GetBegin().x()+raydel.first*mn,ray.GetBegin().y()+raydel.second*mn};
    return ans;
}