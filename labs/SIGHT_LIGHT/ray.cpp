#include "ray.h"
#include <cmath>

static const double DEFAULT_RAY_CAST_LENGTH = 100000.0;

double Ray::distanceSquared(const QPointF& p1, const QPointF& p2) {
    QPointF diff = p1 - p2;
    return QPointF::dotProduct(diff, diff);
}

void Ray::recomputeAngleFromEndpoints() {
    if (distanceSquared(m_begin, m_end) > 1e-12) {
        m_angle_rad = std::atan2(m_end.y() - m_begin.y(), m_end.x() - m_begin.x());
    }
}

void Ray::recomputeEndFromAngle(double length) {
    m_end = QPointF(m_begin.x() + length * std::cos(m_angle_rad),
                    m_begin.y() + length * std::sin(m_angle_rad));
}

Ray::Ray(const QPointF& begin, const QPointF& towards_or_end, double explicit_angle_rad)
    : m_begin(begin)
{
    if (explicit_angle_rad > -999.0) {
        m_angle_rad = explicit_angle_rad;
    } else if (distanceSquared(begin, towards_or_end) > 1e-12) {
        m_angle_rad = std::atan2(towards_or_end.y() - begin.y(), towards_or_end.x() - begin.x());
    } else {
        m_angle_rad = 0.0;
    }
    recomputeEndFromAngle(DEFAULT_RAY_CAST_LENGTH);
}

QPointF Ray::getBegin() const { return m_begin; }
QPointF Ray::getEnd() const { return m_end; }
double Ray::getAngle() const { return m_angle_rad; }


void Ray::setEnd(const QPointF& new_end) {
    m_end = new_end;
    recomputeAngleFromEndpoints();
}

Ray Ray::rotate(double angle_offset_rad) const {
    double new_angle = m_angle_rad + angle_offset_rad;
    while (new_angle <= -M_PI) new_angle += 2 * M_PI;
    while (new_angle > M_PI) new_angle -= 2 * M_PI;
    return Ray(m_begin, QPointF(), new_angle);
}

