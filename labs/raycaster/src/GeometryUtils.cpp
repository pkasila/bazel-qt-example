#include "GeometryUtils.h"

double getDistance(const QPointF& point_1, const QPointF& point_2) {
    return std::hypot(point_1.x() - point_2.x(), point_1.y() - point_2.y());
}

QPointF findClosestPointToTheGiven(const QPointF& givenPoint,
                                   const QVector<QPointF>& points) {
    QPointF closestPoint = points[0];
    double minDistance = getDistance(givenPoint, closestPoint);
    for (const QPointF& point : points) {
        double distance = getDistance(givenPoint, point);
        if (distance < minDistance) {
            minDistance = distance;
            closestPoint = point;
        }
    }
    return closestPoint;
}

double normalizeAngle(double angle) {
    double normalizedAngle = std::fmod(angle, 2 * M_PI);
    if (normalizedAngle < 0) {
        normalizedAngle += 2 * M_PI;
    }
    return normalizedAngle;
}
