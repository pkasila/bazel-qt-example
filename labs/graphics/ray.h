#ifndef RAY_H
#define RAY_H
#include <QPointF>

class Ray {
   public:
    Ray() = delete;
    Ray(const QPointF& begin, const QPointF& end, double angle);
    Ray Rotate(double angle) const;
    const QPointF& GetBegin() const;
    const QPointF& GetEnd() const;
    double GetAngle() const;
    void SetBegin(const QPointF& begin);
    void SetEnd(const QPointF& end);
    void SetAngle(double angle);

   private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};

#endif  // RAY_H
