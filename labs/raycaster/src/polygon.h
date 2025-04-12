#ifndef POLYGON_H
#define POLYGON_H
#include <QPointF>
#include <vector>
#include <optional>
#include "ray.h"

class Polygon
{
public:
    Polygon(const std::vector<QPointF>& vertices);

    std::vector<QPointF> GetVertices() const;

    void AddVertex(const QPointF& vertex);

    void UpdateLastVertex(const QPointF& new_vertex);

    std::optional<QPointF> IntersectRay(const Ray& ray);

    void Complete();

    bool IsComplete() const;

private:
    std::vector<QPointF> vertices;
    bool completed = false;

    std::optional<QPointF> IntersectLine(const QPointF& p1, const QPointF& p2, const Ray& ray){
        QPointF r1 = ray.GetBegin();
        QPointF r2 = ray.GetEnd();

        double Ar = r2.y() - r1.y();
        double Br = r1.x() - r2.x();
        double Cr = r1.x() * r2.y() - r1.y() * r2.x();

        double A = p2.y() - p1.y();
        double B = p1.x() - p2.x();
        double C = p1.x() * p2.y() - p1.y() * p2.x();

        double detD = Ar * B - A * Br;
        if(std::abs(detD) <= 1e-6){
            return std::nullopt;
        }
        double detDx = Cr * B - C * Br;
        double detDy = Ar * C - A * Cr;
        double x = detDx / detD;
        double y = detDy / detD;
        QPointF intersection(x, y);

        auto inBetween = [](double a, double b, double c) {
            return std::min(a, b) - 1e-6 <= c && c <= std::max(a, b) + 1e-6;
        };
        if (!(inBetween(p1.x(), p2.x(), x) && inBetween(p1.y(), p2.y(), y)))
            return std::nullopt;

        QPointF dirRay = r2 - r1;
        QPointF dirInt = intersection - r1;
        double dot = dirRay.x() * dirInt.x() + dirRay.y() * dirInt.y();
        if (dot < 1e-10)
            return std::nullopt;

        return intersection;
    }

};

#endif // POLYGON_H
