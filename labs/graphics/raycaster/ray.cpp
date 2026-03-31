#include "ray.h"
#include <QtMath>
#include <cmath>

const double pi = acos(-1);

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : m_begin(begin), m_end(end), m_angle(angle) {}

Ray::Ray(const QPointF& begin, const QPointF& end)
    : m_begin(begin) {
    const double dx = end.x() - begin.x();
    const double dy = end.y() - begin.y();
    if(dx == 0) {
        m_angle = pi / 2.0;
        if(dy < 0){
            m_angle = - m_angle;
        }
    }
    else if(dy == 0){
        m_angle = pi;
        if(dx > 0){
            m_angle = 0;
        }
    }
    else{
      m_angle = atan2(dy, dx);
    }
    m_end = QPointF(m_begin.x() + cos(m_angle) * 99999, m_begin.y() + sin(m_angle) * 99999);
}

QPointF Ray::getBegin() const { return m_begin; }
QPointF Ray::getEnd() const { return m_end; }
double Ray::getAngle() const { return m_angle; }

void Ray::setBegin(const QPointF& begin) { m_begin = begin; }
void Ray::setEnd(const QPointF& end) { m_end = end; }
void Ray::setAngle(double angle) { m_angle = angle; }

Ray Ray::rotate(double angle) const {
    double dx = m_end.x() - m_begin.x();
    double dy = m_end.y() - m_begin.y();

    double cosA = cos(angle);
    double sinA = sin(angle);

    int newX = m_begin.x() + dx * cosA - dy * sinA;
    int newY = m_begin.y() + dx * sinA + dy * cosA;

    return Ray(m_begin, QPointF(newX, newY), m_angle + angle);
}
