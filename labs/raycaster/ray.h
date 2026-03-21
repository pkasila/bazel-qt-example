#pragma once
#include <QPointF>
#include <cmath>

class Ray {
   public:
    Ray(const QPointF& begin, const QPointF& end, double angle)
        : begin_(begin), end_(end), angle_(angle) {
    }

    QPointF getBegin() const {
        return begin_;
    }

    QPointF getEnd() const {
        return end_;
    }

    double getAngle() const {
        return angle_;
    }

    void setBegin(const QPointF& begin) {
        begin_ = begin;
    }

    void setEnd(const QPointF& end) {
        end_ = end;
    }

    void setAngle(double angle) {
        angle_ = angle;
    }

    Ray Rotate(double angle_offset) const {
        double new_angle = angle_ + angle_offset;
        double len = std::hypot(end_.x() - begin_.x(), end_.y() - begin_.y());
        QPointF new_end(
            begin_.x() + std::cos(new_angle) * len, begin_.y() + std::sin(new_angle) * len);
        return Ray(begin_, new_end, new_angle);
    }

   private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};