#ifndef GEOMETRYUTILS_H
#define GEOMETRYUTILS_H
#include <QPointF>
#include <QVector>
#include <cmath>

double getDistance(const QPointF& point_1, const QPointF& point_2);

QPointF findClosestPointToTheGiven(const QPointF& point,
                                   const QVector<QPointF>& points);

double normalizeAngle(double angle);

#endif // GEOMETRYUTILS_H
