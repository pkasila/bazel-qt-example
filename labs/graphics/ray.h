#ifndef RAY_H
#define RAY_H
#include <QPoint>
#include <QPointF>

class Ray {
public:
    Ray();
    Ray(const QPointF& begin,const QPointF& end, double angle = 0);
    Ray Rotate(double angle) const;

    void SetBegin(QPointF point);
    QPointF Begin() const;
    void SetEnd(QPointF point);
    QPointF End() const;
    void SetDx(double new_dx);
    double Dx() const;
    void SetDy(double new_dy);
    double Dy() const;
    double SquareLength() const;

    double Angle() const;

    bool operator<(const Ray& s) const {
        return angle_ < s.angle_;
    }
private:
    void CalculateAngle();
    QPointF begin_;
    QPointF end_;
    double angle_;
    double dx_;
    double dy_;
    double square_of_length_;
};

#endif  // RAY_H
