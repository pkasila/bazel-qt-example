#include "../include/ray.h"

Ray::Ray(const QPointF& begin, const QPointF& end, double angle) : m_begin(begin), m_end(end), m_angle(angle) {
    updateA();
    updateB();
}

QPointF Ray::getBegin() const {
    return m_begin;
}

QPointF Ray::getEnd() const {
    return m_end;
}

double Ray::getAngle() const {
    return m_angle;
}

void Ray::setBegin(QPointF& begin) {
    m_begin = begin;
    updateA();
    updateB();
}

void Ray::setEnd(QPointF& end) {
    m_end = end;
    updateA();
    updateB();
}

void Ray::setAngle(double angle) {
    m_angle = angle;
    updateA();
    updateB();
}

void Ray::updateA() {
    a = m_end.rx() - m_begin.rx();
}

void Ray::updateB() {
    b = m_end.ry() - m_begin.ry();
}

double Ray::getA() const {
    return a;
}

double Ray::getB() const {
    return b;
}

Ray Ray::Rotate(double angle) {
    double s = std::hypot(m_end.ry() - m_begin.ry(), m_end.rx() - m_begin.rx());
    m_angle = m_angle + angle;
    m_end = m_begin + QPointF(s * std::cos(m_angle), s * std::sin(m_angle));
    updateA();
    updateB();
    return {m_begin, m_end, m_angle};
}

double Distance(QPointF p1, QPointF p2) {
    return std::sqrt(std::pow(p2.x() - p1.x(), 2) + std::pow(p2.y() - p1.y(), 2));
}
