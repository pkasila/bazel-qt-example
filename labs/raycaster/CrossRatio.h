#ifndef CROSSRATIO_H
#define CROSSRATIO_H

#include <QPointF>

float Cross(const QPointF& a, const QPointF& b) {
    return a.x() * b.y() - a.y() * b.x();
}

#endif // CROSSRATIO_H
