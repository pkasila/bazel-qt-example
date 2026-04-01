#ifndef UTILS_H
#define UTILS_H

#include <QPointF>

namespace utils {
    constexpr double kEps = 1e-9;

    bool IsEqual(const double&, const double&);
    bool IsEqual(const QPointF&, const QPointF&);
    bool IsLess(const double&, const double&);
    bool IsMore(const double&, const double&);
    double GetDistance(const QPointF&, const QPointF&);
    double GetAngle(const QPointF&, const QPointF&);
    void Scale(QPointF&, const QPointF&);
    double Sqr(const double&);
}  // namespace utils

#endif  // UTILS_H
