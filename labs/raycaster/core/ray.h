#pragma once

#include <QPointF>

namespace raycaster {

class Ray {
   public:
    Ray(const QPointF& begin, const QPointF& end, double angle);

    const QPointF& GetBegin() const;
    void SetBegin(const QPointF& begin);

    const QPointF& GetEnd() const;
    void SetEnd(const QPointF& end);

    double GetAngle() const;
    void SetAngle(double angle);

    Ray Rotate(double angle) const;

   private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};

}  // namespace raycaster
