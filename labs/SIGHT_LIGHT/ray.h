#ifndef RAY_H
#define RAY_H

#include <QPointF>
#include <cmath>

class Ray
{
public:
    Ray(const QPointF& begin, const QPointF& end, double angle = 0.0);

    QPointF getBegin() const;
    QPointF getEnd() const;
    double getAngle() const;

    void setBegin(const QPointF& begin);
    void setEnd(const QPointF& end);
    void setAngle(double angle);

    Ray rotate(double angle) const;

private:
    QPointF m_begin;
    QPointF m_end;
    double m_angle;
};

#endif // RAY_H
