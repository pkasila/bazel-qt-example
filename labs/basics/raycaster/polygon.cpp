#include "polygon.h"

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    QPointF rayBegin = ray.getBegin();
    QPointF rayEnd = ray.getEnd();
    double rayDx = rayEnd.x() - rayBegin.x();
    double rayDy = rayEnd.y() - rayBegin.y();

    std::optional<QPointF> closestIntersection;
    double minT = std::numeric_limits<double>::max();

    for (size_t i = 0; i < vertices_.size(); ++i) {
        QPointF v1 = vertices_[i];
        QPointF v2 = vertices_[(i + 1) % vertices_.size()];

        double edgeDx = v2.x() - v1.x();
        double edgeDy = v2.y() - v1.y();

        double det = rayDx * edgeDy - rayDy * edgeDx;
        if (std::abs(det) < 1e-10) continue;

        double t = ((v1.x() - rayBegin.x()) * edgeDy - (v1.y() - rayBegin.y()) * edgeDx) / det;
        double u = ((rayBegin.x() - v1.x()) * rayDy - (rayBegin.y() - v1.y()) * rayDx) / -det;

        if (t >= 0 && u >= 0 && u <= 1 && t < minT) {
            minT = t;
            closestIntersection = QPointF(rayBegin.x() + t * rayDx, rayBegin.y() + t * rayDy);
        }
    }

    return closestIntersection;
}