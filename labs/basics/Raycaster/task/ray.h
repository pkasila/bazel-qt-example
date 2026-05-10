#pragma once

#include <QPoint>

class Ray {
   public:
    Ray(const QPoint& begin, const QPoint& end, double angle);

    void SetBegin(const QPoint& begin);
    void SetEnd(const QPoint& end);
    void SetAngle(double angle);

    [[nodiscard]] QPoint GetBegin() const;
    [[nodiscard]] QPoint GetEnd() const;
    [[nodiscard]] double GetAngle() const;

    [[nodiscard]] Ray Rotate(double angle) const;

   private:
    QPoint begin_;
    QPoint end_;
    double angle_;

    static void DefaultRotate(QPoint& point, double angle);
};
