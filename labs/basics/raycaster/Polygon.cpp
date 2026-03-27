
#include "Polygon.h"

#include <limits>

// NOLINTNEXTLINE
std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    QPointF result;
    double min = std::numeric_limits<double>::max();
    bool flag = false;
    QPointF d = ray.GetEnd() - ray.GetBegin();
    QPointF b = ray.GetBegin();
    for (size_t i = 0; i < vertices_.size(); i++) {
        size_t j = (i + 1) % vertices_.size();
        QPointF d2 = vertices_[j] - vertices_[i];
        double det = ((d2.x() * d.y()) - (d.x() * d2.y()));
        if (det != 0) {
            // double t2 = 0;
            double t2 = 1. *
                        ((d.y() * b.x()) - (d.x() * b.y()) + (vertices_[i].y() * d.x()) -
                         (vertices_[i].x() * d.y())) /
                        det;
            double t = 1. *
                       ((d2.y() * vertices_[i].x()) - (d2.x() * vertices_[i].y()) +
                        (b.y() * d2.x()) - (d2.y() * b.x())) /
                       (-det);
            if ((t2 > -0.00001) && (t2 <= 1.000001) && (t >= 0.00001)) {
                if (min > t) {
                    min = t;
                    flag = true;
                }
            }
        }
    }
    if (!flag) {
        return {};
    }
    result = b + (min * d);
    return result;
}
