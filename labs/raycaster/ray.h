#pragma once
#include <QtWidgets>
#include <cmath>

const double kEps = 1e-7;

class Ray{
   public:
    Ray(const QPointF& begin, const QPointF& end, double angle);

    [[nodiscard]] QPointF GetBegin() const;
    void SetBegin(const QPointF& n);
    [[nodiscard]] QPointF GetEnd() const;
    void SetEnd(const QPointF& n);
    [[nodiscard]] double GetAngle() const;
    void SetAngle(const double& n);

    [[nodiscard]] Ray Rotate(double angle) const;

    double Length();
    
   private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};