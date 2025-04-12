#ifndef RAY_H
#define RAY_H
#include <QPointF>

class Ray
{
public:
    Ray(const QPointF& begin, const QPointF& end, double angle);

    QPointF GetBegin() const;

    QPointF GetEnd() const;

    double GetAngle() const;

    Ray Rotate(double angle) const;

    void SetBegin(const QPointF& point);
    void SetEnd(const QPointF& point);


private:
    QPointF begin;
    QPointF end;
    double angle;
};

#endif // RAY_H
