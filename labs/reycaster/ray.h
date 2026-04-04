#ifndef RAY_H
#define RAY_H

#include <QPointF>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle);
    
    // Геттеры
    QPointF getBegin() const { return begin_; }
    QPointF getEnd() const { return end_; }
    double getAngle() const { return angle_; }
    
    // Сеттеры
    void setBegin(const QPointF& begin) { begin_ = begin; }
    void setEnd(const QPointF& end) { end_ = end; }
    void setAngle(double angle) { angle_ = angle; }
    
    Ray Rotate(double angle) const;
    
private:
    QPointF begin_;
    QPointF end_;
    double angle_;
};

#endif // RAY_H
