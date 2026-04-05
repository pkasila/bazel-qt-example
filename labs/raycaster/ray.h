// ray.h
#ifndef RAY_H
#define RAY_H

#include <QPointF>
#include <cmath>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle);
    
    QPointF getBegin() const { return begin_; }
    QPointF getEnd() const { return end_; }
    double getAngle() const { return angle_; }
    
    void setEnd(const QPointF& end) { end_ = end; }
    
    Ray rotate(double angle) const;

private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};

#endif