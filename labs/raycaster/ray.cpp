#include "ray.h"
#include "CrossRatio.h"
#include <QDebug>

Ray::Ray() {}

QPointF& Ray::getBegin() {
    return begin;
}
QPointF& Ray::getEnd() {
    return end;
}
double Ray::getAngle() {
    double angle = std::atan2(end.y() - begin.y(), end.x() - begin.x());
    return (angle < 0) ? angle + 2 * M_PI : angle;
}
void Ray::setBegin(const QPointF& p) {
    begin = p;
}
void Ray::setEnd(const QPointF& p) {
    end = p;
}
void Ray::setAngle(double a) {
    angle = a;
}

std::optional<std::pair<QPointF, float>> Ray::intersection(const QPointF& b, const QPointF& e) const {
    QPointF begi = QPointF(begin.x(), begin.y());
    /*if (begin.x() <= 0) {
        begi.setX(-begin.x());
    }
    if (begin.y() <= 0) {
        begi.setY(-begin.y());
    }*/
    QPointF vec_ray = end - begi;
    QPointF vec_o = e - b;

    if (std::abs(Cross(vec_ray, vec_o)) <= 1e-10) {
        return std::nullopt;
    }

    float x0 = -(begi - b).x();
    float y0 = -(begi - b).y();

    float x1 = vec_ray.x();
    float y1 = vec_ray.y();

    float x2 = vec_o.x();
    float y2 = vec_o.y();

    QPointF T;
    float det = Cross(QPointF(x1, -x2), QPointF(y1, -y2));
    if (std::abs(det) <= 1e-10) {
        return std::nullopt;
    } else {
        T = QPointF(1/det * (x2 * y0 - x0 * y2), 1 / det * (x1 * y0 - y1 * x0));
    }
    if (T.x() >= 0 && T.y() >= 0 && T.y() <= 1) {
        return std::make_pair(begi + T.x() * vec_ray, T.x());
    } else {
        return std::nullopt;
    }
}

Ray Ray::Rotate(double t) const {
    float bx= begin.x();
    float by = begin.y();
    float ex = end.x();
    float ey = end.y();

    Ray r;
    r.setBegin(begin);
    r.setEnd(QPointF((ex - bx) * std::cos(t) + bx - (ey - by) * std::sin(t),
                     (ex - bx) * std::sin(t) + (ey - by) * std::cos(t) + by));

    return r;
}
/*
std::pair<QPointF, float> Ray::intersectionCircle(const QPointF& b, const QPointF& e) const {

}
*/
