#ifndef RAY_H
#define RAY_H

#include <QPointF>

class Ray
{
public:
    Ray();
    Ray(const QPointF& begin, const QPointF& end, double angle);
    Ray Rotate(double angle) const;
    double GetXDir() const;
    double GetYDir() const;
    QPointF GetBegin() const;
    double GetBeginX() const;
    double GetBeginY() const;
    double GetT() const;
    void SetT(double tt);
    QPointF MakeEnd() const;

private:
    QPointF begin;
    double t;
    double x_dir;
    double y_dir;
};

#endif // RAY_H
