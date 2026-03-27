#ifndef RAY_H
#define RAY_H

#include <QPointF>
#include <vector>

class Ray
{
private:
    QPointF begin, end;
    double angle;
public:
    Ray();
    Ray(const QPointF& begin, const QPointF& end, double angle) : begin(begin), end(end), angle(angle) {}
    Ray(const QPointF& begin, const QPointF& end) : begin(begin), end(end) {}
    Ray(const Ray&) = default;
    Ray(Ray&&) = default;
    Ray& operator=(const Ray&) = default;
    Ray& operator=(Ray&&) = default;
    QPointF& getBegin();
    QPointF& getEnd();
    double getAngle();
    void setBegin(const QPointF& p);
    void setEnd(const QPointF& p);
    void setAngle(double a);
    std::optional<std::pair<QPointF, float>> intersection(const QPointF& b, const QPointF& e) const;
    std::pair<QPointF, float> intersectionCircle(const QPointF& center, float) const;
    Ray Rotate(double angle) const;
    std::vector<Ray> CastRays();
};

#endif // RAY_H
