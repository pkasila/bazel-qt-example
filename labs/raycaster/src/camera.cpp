#include "camera.hpp"
#include <QDebug>

QPointF rc::Camera::toGlobal(QPointF point, double height) const { return point * (scale / height) + topLeft; }
QPointF rc::Camera::toLocal(QPointF point, double height) const { return (point - topLeft) * (height / scale); }

void rc::Camera::shift(QPointF delta, double height) {
    topLeft -= delta * (1 / height) * scale;
    normalizeTopLeft();
}

void rc::Camera::rescale(double factor, QPointF mouse, QSizeF size) {
    QPointF oldMousePos = toGlobal(mouse, size.height());

    if(scale * (1 + factor) < 1.0F) {
        scale = 1.0F;
    } else if(scale * (1 + factor) > 10000.0F) {
        scale = 10000.0F;
    } else {
        scale *= (1 + factor);
    }

    QPointF newMousePos = toGlobal(mouse, size.height());
    topLeft += oldMousePos - newMousePos;
    normalizeTopLeft();
}

void rc::Camera::normalizeTopLeft() {
    if(topLeft.x() < -10000) {
        topLeft.setX(-10000);
    }
    if(topLeft.x() > 10000 - scale) {
        topLeft.setX(10000 - scale);
    }
    if(topLeft.y() < -10000) {
        topLeft.setY(-10000);
    }
    if(topLeft.y() > 10000 - scale) {
        topLeft.setY(10000 - scale);
    }
}

QPointF rc::Camera::getTopLeft() const { return topLeft; }
double rc::Camera::getScale() const { return scale; }
