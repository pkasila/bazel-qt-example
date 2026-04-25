#ifndef RAY_H
#define RAY_H
#include <QPointF>
#include <cmath>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle)
        : m_begin(begin), m_end(end), m_angle(angle) {}

    QPointF getBegin() const { return m_begin; }
    QPointF getEnd() const { return m_end; }
    void setEnd(const QPointF& end) { m_end = end; }
    double getAngle() const { return m_angle; }

    Ray Rotate(double angle_rad) const {
        double s = std::sin(angle_rad), c = std::cos(angle_rad);
        double x = m_end.x() - m_begin.x(), y = m_end.y() - m_begin.y();
        return Ray(m_begin, QPointF(x * c - y * s + m_begin.x(), x * s + y * c + m_begin.y()), m_angle + angle_rad);
    }
private:
    QPointF m_begin, m_end;
    double m_angle;
};
#endif