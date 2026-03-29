#ifndef RAY_H
#define RAY_H

#include "functions.h"

#include <QPoint>
#include <cmath>
#include <optional>

namespace RaySegmentNS {

class RaySegment {
   public:
    RaySegment(const QPoint& origin, const QPoint& endpoint, double angle);
    RaySegment(const QPoint& origin, const QPoint& endpoint);
    RaySegment(const QPoint& origin, double angle, double length);
    const QPoint& getStart() const;
    const QPoint& getEnd() const;
    double getDirection() const;
    void setStart(const QPoint& pt);
    void setEnd(const QPoint& pt);
    void setDirection(double angle);
    RaySegment rotated(double delta_angle) const;
    double getLength() const;
    static bool areParallel(const RaySegment& a, const RaySegment& b);

   private:
    QPoint start;
    QPoint end;
    double direction;
};

}  // namespace RaySegmentNS

#endif  // RAY_H
