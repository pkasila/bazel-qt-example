#include "ray.h"


Ray::Ray(const QPointF& begin, const QPointF& end, double angle) : begin_(begin), end_(end), angle_(angle){
}

[[nodiscard]] QPointF Ray::GetBegin() const{
    return begin_;
}
void Ray::SetBegin(const QPointF& n){
    begin_ = n;
}
[[nodiscard]] QPointF Ray::GetEnd() const{
    return end_;
}
void Ray::SetEnd(const QPointF& n){
    end_ = n;
}
[[nodiscard]] double Ray::GetAngle() const{
    return angle_;
}
void Ray::SetAngle(const double& n){
    angle_ = n;
}

[[nodiscard]] Ray Ray::Rotate(double angle) const{
    double r = sqrt(pow(begin_.x() - end_.x(), 2) + pow(begin_.y() - end_.y(), 2));
    double na = angle_ + angle;
    QPointF nend = {begin_.x() + cos(na)*r, begin_.y()+sin(na)*r}; 
    return Ray{begin_,nend,na};
}

double Ray::Length(){
    return sqrt( (begin_.x()-end_.x())*(begin_.x()-end_.x())+(begin_.y()-end_.y())*(begin_.y()-end_.y()) );
}