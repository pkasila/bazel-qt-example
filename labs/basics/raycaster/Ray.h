#pragma once

#include <QPointF>
#include <cmath>

class Ray {
   public:
    Ray(const QPointF& begin, const QPointF& end) : begin_(begin), end_(end) {

    }

    Ray(const Ray&) = default;
    Ray(Ray&&) = default;
    Ray& operator=(const Ray&) = default;
    Ray& operator=(Ray&&) = default;
    ~Ray() = default;

    void SetBegin(const QPointF& begin) {
        begin_ = begin;
    }

    void SetEnd(const QPointF& end) {
        end_ = end;
    }

    [[nodiscard]] QPointF GetBegin() const {
        return begin_;
    }

    [[nodiscard]] QPointF GetEnd() const  {
        return end_;
    }

    [[nodiscard]] Ray Rotate(double angle) const;

   private:
    QPointF begin_;
    QPointF end_;
    // double angle_;
};
