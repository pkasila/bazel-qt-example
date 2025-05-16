#pragma once

#include <QPointF>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle);

    QPointF GetBegin() const;
    QPointF GetEnd() const;
    double GetAngle() const;

    void SetBegin(const QPointF& point);
    void SetEnd(const QPointF& point);

    Ray Rotate(double phi) const;

private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};
