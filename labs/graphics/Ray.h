//
// Created by Kirill Smychok on 6.04.25.
//

#ifndef RAY_H
#define RAY_H
#include <QtCore/QPointF>


class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle);

    QPointF GetBegin() const;
    QPointF GetEnd() const;
    double GetAngle() const;

    void SetBegin(const QPointF& begin);
    void SetEnd(const QPointF& end);
    void SetAngle(double angle);

    Ray Rotate(double angle) const;

private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};



#endif //RAY_H
