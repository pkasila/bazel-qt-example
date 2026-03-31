#ifndef RAY_H
#define RAY_H

#include <QPointF>
#include <cmath>

class Ray
{
public:
    Ray(const QPointF& begin, const QPointF& towards_or_end, double explicit_angle = -1000.0);

    QPointF getBegin() const;
    QPointF getEnd() const;
    double getAngle() const;

    void setEnd(const QPointF& new_end);
    Ray rotate(double angle_offset_rad) const;

private:
    QPointF m_begin;
    QPointF m_end;
    double m_angle_rad;

    static double distanceSquared(const QPointF& p1, const QPointF& p2);
    void recomputeAngleFromEndpoints();
    void recomputeEndFromAngle(double length);
};

#endif // RAY_H

