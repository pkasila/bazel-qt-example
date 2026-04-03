#ifndef RAY_H
#define RAY_H
#pragma once
#include <QPointF>
#include <cmath>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle);
    QPointF getBegin() const;
    QPointF getEnd() const;
    double getAngle() const;
    double getA() const;
    double getB() const;
    void setBegin(QPointF& begin);
    void setEnd(QPointF& end);
    void setAngle(double angle);
    Ray Rotate(double angle);
    void updateA();
    void updateB();

private:
    QPointF m_begin;
    QPointF m_end;
    double m_angle;
    double a;
    double b;
};

double Distance(QPointF p1, QPointF p2);

#endif // RAY_H