#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "ray.h"

#include <QPoint>
#include <QPointF>
#include <cmath>
#include <numbers>
#include <optional>
#include <utility>
#include <vector>

inline double calcDistance(const QPoint& a, const QPoint& b) {
    return std::hypot(a.x() - b.x(), a.y() - b.y());
}

inline double calcDistance(const QPointF& a, const QPointF& b) {
    return std::hypot(a.x() - b.x(), a.y() - b.y());
}

inline double normalizeAngle(double angle) {
    while (angle < 0) {
        angle += 2 * std::numbers::pi;
    }
    return std::fmod(angle, 2 * std::numbers::pi);
}

inline std::optional<std::pair<double, double>> computeIntersectionParams(
    const QPoint& segStart, const QPoint& segEnd, const QPoint& rayOrigin, double ray_dx,
    double ray_dy) {
    double seg_dx = segEnd.x() - segStart.x();
    double seg_dy = segEnd.y() - segStart.y();
    double denominator = ray_dx * seg_dy - ray_dy * seg_dx;
    if (std::abs(denominator) < 1e-9) {
        return std::nullopt;
    }
    double t = ((segStart.x() - rayOrigin.x()) * seg_dy - (segStart.y() - rayOrigin.y()) * seg_dx) /
               denominator;
    double u = ((segStart.x() - rayOrigin.x()) * ray_dy - (segStart.y() - rayOrigin.y()) * ray_dx) /
               denominator;
    return std::make_pair(t, u);
}

#include <algorithm>
#include <ranges>

inline void sortRaySegmentsByDirection(std::vector<RaySegmentNS::RaySegment>* rays) {
    std::ranges::sort(
        *rays, [](const RaySegmentNS::RaySegment& a, const RaySegmentNS::RaySegment& b) {
            return a.getDirection() < b.getDirection();
        });
}

#endif  // FUNCTIONS_H
