#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <QPointF>
#include <QSizeF>

namespace rc {
    class Camera;
}

class rc::Camera {
public:
    [[nodiscard]] QPointF toGlobal(QPointF point, double height) const;
    [[nodiscard]] QPointF toLocal(QPointF point, double height) const;
    void shift(QPointF delta, double height);
    void rescale(double factor, QPointF mouse, QSizeF size);
    [[nodiscard]] QPointF getTopLeft() const;
    [[nodiscard]] double getScale() const;

private:
    void normalizeTopLeft();

    QPointF topLeft{0, 0};
    double scale = 30;
};

#endif // CAMERA_HPP
