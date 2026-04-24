#pragma once
#include <QPointF>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle);
    QPointF getBegin() const;
    QPointF getEnd() const;
    void setEnd(const QPointF& end);
    double getAngle() const;
    Ray Rotate(double angle) const;

private:
    QPointF m_begin;
    QPointF m_end;
    double m_angle;
};