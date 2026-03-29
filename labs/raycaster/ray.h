#include "ray.h"

namespace RaySegmentNS {

RaySegment::RaySegment(const QPoint& origin, const QPoint& endpoint, double angle)
    : start(origin), end(endpoint), direction(normalizeAngle(angle)) {
}

RaySegment::RaySegment(const QPoint& origin, const QPoint& endpoint)
    : start(origin)
    , end(endpoint)
    , direction(normalizeAngle(std::atan2(endpoint.y() - origin.y(), endpoint.x() - origin.x()))) {
}

RaySegment::RaySegment(const QPoint& origin, double angle, double length)
    : start(origin), direction(normalizeAngle(angle)) {
    end = QPoint(
        static_cast<int>(origin.x() + std::cos(angle) * length),
        static_cast<int>(origin.y() + std::sin(angle) * length));
}

const QPoint& RaySegment::getStart() const {
    return start;
}

const QPoint& RaySegment::getEnd() const {
    return end;
}

double RaySegment::getDirection() const {
    return direction;
}

void RaySegment::setStart(const QPoint& pt) {
    start = pt;
}

void RaySegment::setEnd(const QPoint& pt) {
    end = pt;
}

void RaySegment::setDirection(double angle) {
    direction = normalizeAngle(angle);
}

RaySegment RaySegment::rotated(double delta_angle) const {
    double newAngle = normalizeAngle(direction + delta_angle);
    double len = getLength();
    return RaySegment(start, newAngle, len);
}

double RaySegment::getLength() const {
    return calcDistance(start, end);
}

bool RaySegment::areParallel(const RaySegment& a, const RaySegment& b) {
    QPoint vecA = a.end - a.start;
    QPoint vecB = b.end - b.start;
    double cross = vecA.x() * vecB.y() - vecA.y() * vecB.x();
    return std::abs(cross) < 1e-9;
}

}  // namespace RaySegmentNS
