#include "ray.h"

#include <cmath>


Ray::Ray(){

}

Ray::Ray(const QPointF& begin,const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(angle){
    dy_ = end_.y() - begin_.y();
    dx_ = end_.x() - begin_.x();
    square_of_length_ = dy_*dy_ + dx_*dx_;
    CalculateAngle();
}

Ray Ray::Rotate(double angle) const {
    Ray ray;
    ray.angle_ = this->angle_ + angle;
    ray.begin_ = this->begin_;
    ray.dx_ = dx_*cos(angle) + dy_*sin(angle);
    ray.dy_ = -dx_*sin(angle) + dy_*cos(angle);
    ray.dx_ *= 10000;
    ray.dy_ *= 10000;
    ray.square_of_length_ = ray.dx_*ray.dx_ + ray.dy_*ray.dy_;
    ray.end_ = QPointF(ray.begin_.x() + ray.dx_, ray.begin_.y() + ray.dy_);
    return ray;
}

void Ray::SetBegin(QPointF point) {
    begin_ = point;
}

QPointF Ray::Begin() const {
    return begin_;
}
void Ray::SetEnd(QPointF point) {
    end_ = point;
    dx_ = end_.x() - begin_.x();
    dy_ = end_.y() - begin_.y();
    CalculateAngle();
}

QPointF Ray::End() const {
    return end_;
}

void Ray::SetDx(double new_dx) {
    dx_ = new_dx;
    end_.setX(begin_.x() + dx_);
    CalculateAngle();
}

double Ray::Dx() const {
    return dx_;
}

void Ray::SetDy(double new_dy) {
    dy_ = new_dy;
    end_.setY(begin_.y() + dy_);
    CalculateAngle();
}

double Ray::Dy() const {
    return dy_;
}

double Ray::SquareLength() const {
    return square_of_length_;
}

void Ray::CalculateAngle() {
    angle_ = atan2(dy_, dx_);
}

double Ray::Angle() const {
    return angle_;
}

