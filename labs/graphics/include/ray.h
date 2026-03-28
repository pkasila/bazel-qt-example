#ifndef RAY_H
#define RAY_H

#include <QPointF>
#include <cmath>

class Ray {
   public:
    Ray(const QPointF& begin, const QPointF& end, double angle);
    [[nodiscard]] QPointF GetBegin() const;
    [[nodiscard]] QPointF GetEnd() const;
    [[nodiscard]] double GetAngle() const;
    void SetBegin(const QPointF& begin);
    void SetEnd(const QPointF& end);
    void SetAngle(double angle);
    [[nodiscard]] Ray Rotate(double angle) const;

    bool operator==(const Ray& other) const {
        return begin_ == other.begin_ && end_ == other.end_;
    }

   private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};
#endif  // RAY_H
