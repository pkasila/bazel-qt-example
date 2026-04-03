#ifndef RAY_H
#define RAY_H

#include <QPointF>
#include <vector>
class Ray
{
public:
    Ray(const QPointF& begin, const QPointF& end);

    QPointF GetBegin() const;
    QPointF GetEnd() const;
    double GetAngle() const;


    void SetBegin(const QPointF& newBegin);
    void SetEnd(const QPointF& newEnd);

    Ray Rotate(double phi) const;
private:
    QPointF begin_;
    QPointF end_;
    double angle_;

    void CalculateAngle();
};

#endif // RAY_H
