#pragma once

#include <QPointF>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle);
    Ray(const Ray& ray);

    const QPointF& GetBegin() const;
    const QPointF& GetEnd() const;
    double GetAngle() const;

    void SetBegin(const QPointF& begin);
    void SetEnd(const QPointF& end);
    void SetAngle(double angle);

    Ray Rotate(double angle) const;

private:
    QPointF begin_;
    QPointF end_;
    double angle_;

    double SolveAngle() const;
};