#ifndef UTIL_HPP
#define UTIL_HPP

#include <QPointF>

inline double distance(const QPointF& a, const QPointF& b) {
    return std::sqrt((b.x() - a.x()) * (b.x() - a.x()) + (b.y() - a.y()) * (b.y() - a.y()));
}

inline std::optional<QPointF> intersectSegments(
    const QPointF& a, const QPointF& b, const QPointF& c, const QPointF& d) {
    double x1 = a.x(), y1 = a.y();
    double x2 = b.x(), y2 = b.y();
    double x3 = c.x(), y3 = c.y();
    double x4 = d.x(), y4 = d.y();
    double pxA = (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4);
    double pxB = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    double pyA = (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);
    double pyB = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    QPointF res(pxA / pxB, pyA / pyB);
    double eps = 1e-5;
    if(std::min(x1, x2) - eps <= res.x() && res.x() <= std::max(x1, x2) + eps && std::min(x3, x4) - eps <= res.x() &&
        res.x() <= std::max(x3, x4) + eps && std::min(y1, y2) - eps <= res.y() && res.y() <= std::max(y1, y2) + eps &&
        std::min(y3, y4) - eps <= res.y() && res.y() <= std::max(y3, y4) + eps) {
        return res;
    }
    return std::nullopt;
}

#endif // UTIL_HPP
