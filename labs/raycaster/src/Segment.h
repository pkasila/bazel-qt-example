#ifndef SEGMENT_H
#define SEGMENT_H
//Qt libraries
#include <QPointF>
//C++ libraries
#include <optional>
//Custom libraries
#include "ProjectConstants.h"
#include "Ray.h"
#include "GeometryUtils.h"

class Segment {
public:
    //Constructors
    Segment(const QPointF& beginPoint, const QPointF& endPoint);

    //Getters
    QPointF getBeginPoint() const;
    QPointF getEndPoint() const;

    //Methods
    std::optional<QPointF> getPointOnSegment (double t) const;
    std::optional<QPointF> findIntersectionWithRay(const Ray& ray) const;
    Ray getRayBasedOnSegment() const;
    bool isPointOnSegment(const QPointF& point) const;
    std::optional<QPointF> findIntersectionWithSegment(const Segment& anotherSegment) const;

private:
    QPointF beginPoint;
    QPointF endPoint;
};

#endif // SEGMENT_H
