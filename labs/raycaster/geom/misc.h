#ifndef MISC_H
#define MISC_H

#include <QPointF>

namespace misc {
    constexpr double kEPS0 = 1e-2;
    constexpr double kEPS1 = 1e-4;
    constexpr double kEPS2 = 1e-9;
    constexpr double kINF = 1e6;
    double DotProduct(const QPointF& v1, const QPointF& v2);
    double Length(const QPointF& p1, const QPointF& p2);
    double Length(const QPointF& v1);
    double CrossProductValue(const QPointF& v1, const QPointF& v2);
    double Angle(const QPointF& v1);
    bool IsCollinear(const QPointF& v1, const QPointF& v2);
} // namespace misc

#endif
