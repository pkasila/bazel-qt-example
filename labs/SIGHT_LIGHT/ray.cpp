#include "ray.h"
#include <cmath>

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : m_begin(begin), m_end(end), m_angle(angle)
{
}

QPointF Ray::getBegin() const { return m_begin; }
QPointF Ray::getEnd() const { return m_end; }
double Ray::getAngle() const { return m_angle; }

void Ray::setBegin(const QPointF& begin) { m_begin = begin; }
void Ray::setEnd(const QPointF& end) { m_end = end; }
void Ray::setAngle(double angle) { m_angle = angle; }



// Ray Ray::rotate(double phi) const {
//     double len = std::hypot(m_end.x() - m_begin.x(), m_end.y() - m_begin.y());
//     QPointF new_end(
//         m_begin.x() + len * 1000 * std::cos(m_angle + phi),
//         m_begin.y() + len * 1000 * std::sin(m_angle + phi));
//     return Ray(m_begin, new_end);
// }

// Ray Ray::rotate(const double angle) const {
//     const double angle2 = m_angle + angle;
//     const double dx = cosl(angle2);
//     const double dy = sinl(angle2);
//     const double h = std::hypot(m_end.x() - m_begin.x(), m_end.y() - m_begin.y());
//     const QPoint end = {m_begin.x() + h * dx, m_begin.y() - h * dy};
//     return Ray(m_begin, end);
// }

// Ray Ray::rotate(const double angle) const {
//     const double angle2 = m_angle + angle;
//     const double dx = cosl(angle2);
//     const double dy = sinl(angle2);
//     const double h = std::hypot(m_end.x() - m_begin.x(), m_end.y() - m_begin.y());
//     const QPointF end = {m_begin.x() + h * dx, m_begin.y() - h * dy};
//     return {m_begin, end, angle2};
// }

Ray Ray::rotate(double angle) const
{
    //double new_angle = m_angle + angle;
    double dx = m_end.x() - m_begin.x();
    double dy = m_end.y() - m_begin.y();

    double cos_a = cos(angle);
    double sin_a = sin(angle);

    int new_x = m_begin.x() + dx * cos_a - dy * sin_a;
    int new_y = m_begin.y() + dx * sin_a + dy * cos_a;


    return Ray(m_begin, QPointF((new_x-m_begin.x())*1000+new_x, (new_y-m_begin.y())*1000+new_y));
}

// Ray Ray::rotate(double angle) const {
//     QPoint delta = m_end - m_begin;
//     const QPoint matrix[2] = {QPoint(cos(angle), -sin(angle)), QPoint(sin(angle), cos(angle))};
//     delta = QPoint(QPoint::dotProduct(matrix[0], delta), QPoint::dotProduct(matrix[1], delta));
//     return Ray(m_begin, m_begin + delta, m_angle + angle);
// }
