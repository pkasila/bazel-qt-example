#pragma once

#include <QPoint>
#include <QPointF>
#include <cmath>

class Ray {
public:
    Ray() = default;
    Ray(const QPoint& begin, const QPoint& end, double angle);
    Ray(const QPointF& begin, const QPointF& end, double angle);

    [[nodiscard]] QPointF getBegin() const { return begin_; }
    [[nodiscard]] QPointF getEnd() const { return end_; }
    [[nodiscard]] double getAngle() const { return angle_; }

    void setBegin(const QPointF& begin) { begin_ = begin; }
    void setEnd(const QPointF& end) { end_ = end; }
    void setAngle(double angle) { angle_ = angle; }

    [[nodiscard]] Ray Rotate(double angle) const;

    [[nodiscard]] double length() const;

private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};