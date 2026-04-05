#include "polygon.h"

std::optional<QPointF> Polygon::intersectRay(const Ray& ray) const {
    std::optional<QPointF> closestPoint = std::nullopt;
    double minT = 1.0;

    QPointF p1 = ray.getBegin();
    QPointF p2 = ray.getEnd();
    double dx12 = p2.x() - p1.x();
    double dy12 = p2.y() - p1.y();

    for (size_t i = 0; i < vertices_.size(); ++i) {
        QPointF p3 = vertices_[i];
        QPointF p4 = vertices_[(i + 1) % vertices_.size()];
        
        double dx34 = p4.x() - p3.x();
        double dy34 = p4.y() - p3.y();
        
        double det = dx12 * (-dy34) - (-dx34) * dy12;
        if (std::abs(det) < 1e-9) continue;

        double t = ((p3.x() - p1.x()) * (-dy34) - (-dx34) * (p3.y() - p1.y())) / det;
        double u = (dx12 * (p3.y() - p1.y()) - (p3.x() - p1.x()) * dy12) / det;

        if (t >= 0 && t <= 1.0 && u >= 0 && u <= 1.0) {
            if (!closestPoint || t < minT) {
                minT = t;
                closestPoint = QPointF(p1.x() + t * dx12, p1.y() + t * dy12);
            }
        }
    }
    return closestPoint;
}