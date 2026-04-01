#ifndef RAY_H
#define RAY_H

#include <QPointF>
#include <cmath>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle) 
        : begin_(begin), end_(end), angle_(angle) {}

    QPointF getBegin() const { return begin_; }
    QPointF getEnd() const { return end_; }
    double getAngle() const { return angle_; }

    void setBegin(const QPointF& begin) { begin_ = begin; }
    void setEnd(const QPointF& end) { end_ = end; }
    void setAngle(double angle) { angle_ = angle; }

    Ray Rotate(double angle) const {
        double newAngle = angle_ + angle;
        double length = std::sqrt(std::pow(end_.x() - begin_.x(), 2) + std::pow(end_.y() - begin_.y(), 2));
        QPointF newEnd(
            begin_.x() + length * std::cos(newAngle),
            begin_.y() + length * std::sin(newAngle)
        );
        return Ray(begin_, newEnd, newAngle);
    }

private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};

#endif