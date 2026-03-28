#ifndef RAY_H
#define RAY_H

#include <QPoint>
#include <cmath>

class Ray {
public:
    Ray(const QPoint& begin, const QPoint& end, double angle);

    QPoint getBegin() const { return begin_; }
    QPoint getEnd() const { return end_; }
    double getAngle() const { return angle_; }

    void setBegin(const QPoint& begin) { begin_ = begin; }
    void setEnd(const QPoint& end) { end_ = end; }
    void setAngle(double angle) { angle_ = angle; }
    
    Ray Rotate(double angle) const;
    
private:
    QPoint begin_;
    QPoint end_;
    double angle_;
};

#endif // RAY_H
