#include "Segment.h"

//Constructors
Segment::Segment(const QPointF& beginPoint, const QPointF& endPoint) :
    beginPoint(beginPoint), endPoint(endPoint) {}

//Getters
QPointF Segment::getBeginPoint() const {
    return this->beginPoint;
}

QPointF Segment::getEndPoint() const {
    return this->endPoint;
}

//Methods
std::optional<QPointF> Segment::getPointOnSegment (double t) const {
    //Segment equations:
    //x(t) = x_a + t(x_b - x_a)
    //y(t) = y_a + t(y_b - y_a), 0 <= t <= 1
    if (!(-Geometry::EPSILON <= t && t <= 1.0 + Geometry::EPSILON)) {
        return std::nullopt;
    }

    double x_a = beginPoint.x();
    double y_a = beginPoint.y();

    double x_b = endPoint.x();
    double y_b = endPoint.y();

    QPointF pointOnSegment = QPointF(x_a + t * (x_b - x_a),
                                     y_a + t * (y_b - y_a));
    return pointOnSegment;
}

std::optional<QPointF> Segment::findIntersectionWithRay(const Ray& ray) const {
    //Ray direction vector
    Vector2D a = ray.getDirectionVector();
    double a_x = a.getXCord();
    double a_y = a.getYCord();

    QPointF rayBeginPoint = ray.getBeginPoint();
    double x_0 = rayBeginPoint.x();
    double y_0 = rayBeginPoint.y();

    //Segment cords
    double x_a = beginPoint.x();
    double y_a = beginPoint.y();
    double x_b = endPoint.x();
    double y_b = endPoint.y();

    //If denom == 0, ray and segment are parallel
    double denom = a_y * (x_b - x_a) - a_x * (y_b - y_a);
    if (std::abs(denom) < Geometry::EPSILON) {
        return std::nullopt;
    }

    //Parameter t_1 reflects intersection position on the segment
    double t_1 = (a_x * (y_a - y_0) - a_y * (x_a - x_0)) / denom;

    //If t_1 < 0.0 or t_1 > 1.0, the point does not lie on the segment.
    if ((t_1 < -Geometry::EPSILON) || (t_1 > 1.0 + Geometry::EPSILON)) {
        return std::nullopt;
    }

    //Possible intersection point cords
    double interX = x_a + t_1 * (x_b - x_a);
    double interY = y_a + t_1 * (y_b - y_a);

    double t; //shows if possible intersection point corresponds to the ray direction
    if (std::abs(a_x) > Geometry::EPSILON) {
        t = (interX - x_0) / a_x;
    } else if (std::abs(a_y) > Geometry::EPSILON) {
        t = (interY - y_0) / a_y;
    } else {
        return std::nullopt;
    }

    //If t > 0.0, we found the intersection
    if (t > Geometry::EPSILON) {
        return QPointF(interX, interY);
    } else {
        return std::nullopt;
    }
}

Ray Segment::getRayBasedOnSegment() const {
    return Ray(this->beginPoint, this->endPoint);
}

bool Segment::isPointOnSegment(const QPointF& point) const {
    //Get segment data
    double x_a = this->beginPoint.x();
    double x_b = this->endPoint.x();

    double y_a = this->beginPoint.y();
    double y_b = this->endPoint.y();

    double dx = x_b - x_a;
    double dy = y_b - y_a;

    double dx_p = point.x() - x_a;
    double dy_p = point.y() - y_a;

    //Collinearity check
    double cross = dx * dy_p - dy * dx_p;
    if (std::abs(cross) > Geometry::EPSILON)
        return false;

    //Calculating parameter t
    double t;
    if (std::abs(dx) > std::abs(dy)) {
        t = dx_p / dx;
    } else {
        t = dy_p / dy;
    }

    //t must lie between 0.0 and 1.0
    if ((-Geometry::EPSILON <= t) && (t <= 1.0 + Geometry::EPSILON)) {
        return true;
    } else {
        return false;
    }
}

std::optional<QPointF> Segment::findIntersectionWithSegment(const Segment& anotherSegment) const {
    Ray rayBasedOnThisSegment = this->getRayBasedOnSegment();
    std::optional<QPointF> possibleIntersection = anotherSegment.findIntersectionWithRay(rayBasedOnThisSegment);
    if (possibleIntersection) {
        if (this->isPointOnSegment(*possibleIntersection)) {
            return possibleIntersection;
        } else {
            return std::nullopt;
        }
    } else {
        return std::nullopt;
    }
}
