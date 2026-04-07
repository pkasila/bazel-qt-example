#ifndef RAY_H
#define RAY_H

#include <QPoint>
#include <cmath>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle);
    QPointF GetBegin() const;
    QPointF GetEnd() const;
    double GetAngle() const;
    void SetBegin(const QPointF& begin);
    void SetEnd(const QPointF& end);
    void SetAngle(double angle);
    Ray Rotate(double angle) const;

    bool operator==(const Ray& other) const {
        return begin_ == other.begin_ && end_ == other.end_;
    }

private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};
#endif
