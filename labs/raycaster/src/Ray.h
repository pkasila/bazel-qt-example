#ifndef RAY_H
#define RAY_H
//Qt libraries
#include <QPointF>
#include <optional>
//C++ libraries
#include <cmath>
//Custom libraries
#include "Vector2D.h"
#include "GeometryUtils.h"

class Ray {
public:
    //Constructors
    Ray(const QPointF& beginPoint, const QPointF& endPoint);
    Ray(const QPointF& beginPoint, const Vector2D& directionVector);
    Ray();
    //Setters
    void setEndPoint(const QPointF& newEndPoint);

    //Getters
    QPointF getBeginPoint() const;
    std::optional<QPointF> getEndPoint() const;
    Vector2D getDirectionVector() const;

    //Public methods
    Ray rotate(double rotationAngle) const;
    bool isEndPointDetermined() const;

private:
    QPointF beginPoint;
    std::optional<QPointF> endPoint;
    Vector2D directionVector;
};

#endif // RAY_H
