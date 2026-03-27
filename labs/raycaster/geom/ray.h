#ifndef RAY_H
#define RAY_H

#include <QPointF>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle);
    [[nodiscard]] QPointF GetBegin() const;
    [[nodiscard]] QPointF GetEnd() const;
    [[nodiscard]] double GetAngle() const;
    void SetBegin(const QPointF& begin);
    void SetEnd(const QPointF& end);
    void SetAngle(double angle);
    [[nodiscard]] Ray Scale(double scale) const ;
    [[nodiscard]] Ray Rotate(double angle) const;
private: 
    QPointF begin_;
    QPointF end_;
    double angle_;
};
#endif