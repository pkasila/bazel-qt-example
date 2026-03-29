//
// Created by Zazerg on 3/31/25.
//

#ifndef RAY_H
#define RAY_H
#include <qpoint.h>


class Ray {
public:
    Ray (const QPointF &origin, const QPointF &direction, bool segment) : origin_ (origin), direction_ (direction), segment_ (segment) {
    }

    const QPointF &origin () const {
        return origin_;
    }

    const QPointF &direction () const {
        return direction_;
    }

    Ray rotate (double rx, double ry) const {
        return Ray(this->origin_, origin_ + QPointF((direction_-origin_).x()*rx-(direction_-origin_).y()*ry, (direction_-origin_).x()*ry + (direction_-origin_).y()*rx), false);
    }

    bool isSegment() const {return segment_;}

    void setSegment (bool seg) {segment_ = seg;}

    std::optional<QPointF> Intersect(const Ray& other) const {return IntersectMath(other, segment_, other.isSegment());}

    void setDirection(QPointF value);

private:
    std::optional<QPointF> IntersectMath (const Ray& other, bool check1, bool check2) const {
        auto v1 = this->direction() - this->origin();
        auto v2 = other.direction() - other.origin();
        auto d = this->origin() - other.origin();
        double dn = (v1.x() * v2.y() - v2.x() * v1.y());
        if (abs(dn) < 1e-8) return std::nullopt;
        double t = (- d.x() * v2.y() + d.y() * v2.x()) / dn;
        if (t < 0 || (check1 && t > 1)) return std::nullopt;
        double u = (d.y() * v1.x() - d.x() * v1.y()) / dn;
        if (u < 0 || (check2 && u > 1)) return std::nullopt;
        return this->origin() + v1 * t;
    }

    QPointF origin_;
    QPointF direction_;
    bool segment_;
};



#endif //RAY_H
