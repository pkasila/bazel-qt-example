#include "controller.h"
#include "ray.h"
#include <algorithm>
#include <cmath>
#include <iostream>

std::vector<Polygon>& Controller::GetPolygons(){
    return polygons_;
}
void Controller::AddPolygon(const Polygon& poly){
    polygons_.push_back(poly);
}

double IntersectionSegments(QPointF p1, QPointF p2, QPointF b, QPointF e){
    std::pair<double, double> del1 = {p2.x()-p1.x(),p2.y()-p1.y()};
    std::pair<double, double> del2 = {e.x()-b.x(),e.y()-b.y()};
    if (abs(del2.first*del1.second - del2.second*del1.first) < kEps) {
        return -1;
    }
    double t2 = (del1.first*(b.y()-p1.y()) + del1.second*(p1.x()-b.x()))/(del2.first*del1.second - del2.second*del1.first);
    double t1=0;
    if (abs(del1.first)<kEps){
        t1 = (b.y()+del2.second*t2-p1.y())/del1.second;
    }else{
        t1 = (b.x()+del2.first*t2-p1.x())/del1.first;
    }
    if (kEps<t1 && t1 < 1-kEps && -kEps<t2 && t2<1+kEps) {
        return t1;
    }
    return -1;
    
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    std::vector<double> v;
    if (polygons_.back().Get().empty()){
        polygons_.back().AddVertex(new_vertex);
        return;
    }
    QPointF old_vertex = polygons_.back().Get().back();
    for (int i=0; i+1 < polygons_.size(); i++){
        for (int j=0; j < polygons_[i].Get().size(); j++){
            double f = IntersectionSegments(old_vertex, new_vertex, polygons_[i].Get()[j], polygons_[i].Get()[(j+1)%polygons_[i].Get().size()]);
            if (f>kEps && f<1-kEps){
                v.push_back(f);
            }
        }
    }
    for (int j=0; j+1 < polygons_.back().Get().size(); j++){
        double f = IntersectionSegments(old_vertex, new_vertex, polygons_.back().Get()[j], polygons_.back().Get()[j+1]);
        if (f>kEps && f<1-kEps){
            v.push_back(f);
        }
    }
    sort(v.begin(), v.end());
    for (auto u : v){
        std::pair<double,double> del = {new_vertex.x()-old_vertex.x(),new_vertex.y()-old_vertex.y()};
        polygons_.back().AddVertex({old_vertex.x()+del.first*u,old_vertex.y()+del.second*u});
    }
    if (polygons_.back().Get()[0] != new_vertex){
        polygons_.back().AddVertex(new_vertex);
    }
    
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()){
        polygons_.back().UpdateLastVertex(new_vertex);
    }
}

void Controller::SetLight(const QPointF& ls){
    light_source_ = ls;
}
QPointF Controller::GetLight(){
    return light_source_;
}
int& Controller::Mode(){
    return mode_;
}

std::vector<Ray> Controller::CastRays(){
    std::vector<Ray> ans;
    for (auto poly : polygons_){
        std::vector<QPointF> vtx = poly.Get();
        for (auto v : vtx){
            double ang = std::atan2(v.y()-light_source_.y(),v.x()-light_source_.x());
            Ray r(light_source_, v, ang);
            ans.push_back(r);
            Ray rr = r.Rotate(-0.0001);
            if (polygons_[0].IntersectRay(rr).has_value()){
                rr.SetEnd(polygons_[0].IntersectRay(rr).value());
            }
            ans.push_back(rr);
            Ray rrr = r.Rotate(0.0001);
            if (polygons_[0].IntersectRay(rrr) != std::nullopt){
                rrr.SetEnd(polygons_[0].IntersectRay(rrr).value());
            }
            ans.push_back(rrr);
        }
    }
    std::sort(ans.begin(),ans.end(),[](Ray r1, Ray r2){return r1.GetAngle()<r2.GetAngle();});
    return ans;
}

void Controller::IntersectRays(std::vector<Ray>* rays){
    for (auto& r : *rays){
        for (auto& p : polygons_){
            std::optional<QPointF> nend = p.IntersectRay(r);
            if (nend.has_value()){
                QPointF ee = nend.value();
                if ( sqrt((ee.x()-r.GetBegin().x())*(ee.x()-r.GetBegin().x()) + (ee.y()-r.GetBegin().y())*(ee.y()-r.GetBegin().y())) < r.Length() ){
                    r.SetEnd(ee);
                }                
            }

        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays){
    std::vector<Ray> rr = *rays;
    std::vector<Ray> ans; ans.clear();
    for (int i=0;i<rays->size();i++){
        if (ans.empty() || Ray(rr[i].GetEnd(),ans.back().GetEnd(),0).Length()>kEps){
            ans.push_back(rr[i]);
        }
    }
    *rays = ans;
}

Polygon Controller::CreateLightArea(){
    std::vector<Ray> vr = CastRays();
    IntersectRays(&vr);
    RemoveAdjacentRays(&vr);
    Polygon poly({});
    for (auto r : vr){
        poly.AddVertex(r.GetEnd());
    }
    return poly;
}


void Controller::AddStaticLight(QPointF light){
    st_lights_.push_back(light);
}

std::vector<QPointF>& Controller::GetStaticLights(){
    return st_lights_;
}