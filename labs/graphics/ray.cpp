#include "ray.h"
#include <cmath>

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : m_begin(begin), m_end(end), m_angle(angle) {}

QPointF Ray::getBegin() const { return m_begin; }
QPointF Ray::getEnd() const { return m_end; }
void Ray::setEnd(const QPointF& end) { m_end = end; }
double Ray::getAngle() const { return m_angle; }

Ray Ray::Rotate(double angle) const {
    double newAngle = m_angle + angle;
    double dx = m_end.x() - m_begin.x();
    double dy = m_end.y() - m_begin.y();
    double len = std::sqrt(dx * dx + dy * dy);
    return Ray(m_begin, QPointF(m_begin.x() + std::cos(newAngle) * len, 
    m_begin.y() + std::sin(newAngle) * len), newAngle);
}