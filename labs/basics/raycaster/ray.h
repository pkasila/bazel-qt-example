#ifndef RAY_H
#define RAY_H

#include <QPointF>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle);
    QPointF getBegin() const;
    QPointF getEnd() const;
    double getAngle() const;
    void setEnd(const QPointF& end);
    Ray rotate(double angle) const;

private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};

#endif // RAY_H
