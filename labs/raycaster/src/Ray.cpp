#include "Ray.h"
//Constructors
Ray::Ray(const QPointF& beginPoint, const QPointF& endPoint) :
    beginPoint(beginPoint), endPoint(endPoint) {
    double directionVectorXCord = (endPoint.x() - beginPoint.x()) / getDistance(beginPoint, endPoint);
    double directionVectorYCord = (endPoint.y() - beginPoint.y()) / getDistance(beginPoint, endPoint);
    directionVector = Vector2D(directionVectorXCord, directionVectorYCord);
}

Ray::Ray(const QPointF& beginPoint, const Vector2D& directionVector) :
    beginPoint(beginPoint), endPoint(std::nullopt), directionVector(directionVector) {}

Ray::Ray() : Ray(QPointF(), Vector2D(1.0, 0.0)) {}

//Setters
void Ray::setEndPoint(const QPointF& newEndPoint) {
    //Function is not called when directionVector may be changed
    this->endPoint = newEndPoint;
}

//Getters
QPointF Ray::getBeginPoint() const {
    return this->beginPoint;
}

Vector2D Ray::getDirectionVector() const {
    return this->directionVector;
}

std::optional<QPointF> Ray::getEndPoint() const {
    return this->endPoint;
}

//Public methods
Ray Ray::rotate(double rotationAngle) const {
    double dirVectorXCord = directionVector.getXCord();
    double dirVectorYCord = directionVector.getYCord();
    double newDirVectorXCord = dirVectorXCord * std::cos(rotationAngle) -
                               dirVectorYCord * std::sin(rotationAngle);
    double newDirVectorYCord = dirVectorXCord * std::sin(rotationAngle) +
                               dirVectorYCord * std::cos(rotationAngle);
    return Ray(beginPoint, Vector2D(newDirVectorXCord, newDirVectorYCord));
}

bool Ray::isEndPointDetermined() const {
    return endPoint.has_value();
}
