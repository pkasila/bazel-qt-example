#ifndef RAY_HPP
#define RAY_HPP

#include <QPointF>
#include <QVector2D>

namespace rc {
    class Ray;
}

class rc::Ray {
public:
    Ray(const QPointF& origin, const QVector2D& direction);
    Ray(const QPointF& begin, const QPointF& end);
    [[nodiscard]] QPointF getOrigin() const;
    [[nodiscard]] QVector2D getDirection() const;
    [[nodiscard]] double getAngle() const;
    [[nodiscard]] Ray rotate(double angle) const;

private:
    QPointF origin;
    QVector2D direction;
};

#endif // RAY_HPP
