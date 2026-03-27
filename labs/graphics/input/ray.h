#pragma once

#include <QPointF>
#include <cmath>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end);
    Ray(const QPointF& begin, double angle, double length);

    QPointF getBegin() const { return begin_; }
    QPointF getEnd() const { return end_; }
    double getAngle() const;
    double getLength() const;

    void setBegin(const QPointF& begin) { begin_ = begin; }
    void setEnd(const QPointF& end) { end_ = end; }

    Ray Rotate(double angle_rad) const;

private:
    QPointF begin_;
    QPointF end_;

    QPointF getVector() const { return end_ - begin_; }
};

inline double distSq(const QPointF& p1, const QPointF& p2) {
    const double dx = p1.x() - p2.x();
    const double dy = p1.y() - p2.y();
    return dx * dx + dy * dy;
}