#include "controller.h"

#include <algorithm>
#include <limits>
#include <optional>

RaycasterController::RaycasterController()
    : lightPos(0, 0), currentMode(RenderMode::Light), constructing(false) {
    PolygonShapeNS::PolygonShape border(
        {QPoint(0, 0), QPoint(800, 0), QPoint(800, 600), QPoint(0, 600)});
    polygonList.push_back(border);
}

void RaycasterController::beginPolygon(const QPoint& initPt) {
    currentPolygon = PolygonShapeNS::PolygonShape();
    currentPolygon.addVertex(initPt);
    currentPolygon.addVertex(initPt);
    polygonList.push_back(currentPolygon);
    constructing = true;
}

void RaycasterController::appendVertex(const QPoint& pt) {
    if (!polygonList.empty()) {
        polygonList.back().addVertex(pt);
    }
}

void RaycasterController::updateCurrentPolygon(const QPoint& pt) {
    if (!polygonList.empty()) {
        polygonList.back().updateLastVertex(pt);
    }
}

void RaycasterController::completePolygon() {
    constructing = false;
}

const std::vector<PolygonShapeNS::PolygonShape>& RaycasterController::getPolygons() const {
    return polygonList;
}

QPoint RaycasterController::getLightPosition() const {
    return lightPos;
}

void RaycasterController::setLightPosition(const QPoint& pt) {
    lightPos = pt;
}

std::vector<RaySegmentNS::RaySegment> RaycasterController::generateLightRays(
    const QPoint& srcPos) const {
    std::vector<RaySegmentNS::RaySegment> rays;
    for (const auto& poly : polygonList) {
        for (const auto& vertex : poly.getVertices()) {
            RaySegmentNS::RaySegment baseRay(srcPos, vertex);
            rays.push_back(baseRay);
            rays.push_back(baseRay.rotated(-GlobalConfig::ROTATION_DELTA));
            rays.push_back(baseRay.rotated(GlobalConfig::ROTATION_DELTA));
        }
    }
    sortRaySegmentsByDirection(&rays);
    return rays;
}

std::vector<RaySegmentNS::RaySegment> RaycasterController::generateLightRays() const {
    return generateLightRays(lightPos);
}

void RaycasterController::processRayIntersections(
    std::vector<RaySegmentNS::RaySegment>* rays) const {
    for (auto& ray : *rays) {
        double minDist = std::numeric_limits<double>::infinity();
        std::optional<QPoint> bestIntersection;
        for (const auto& poly : polygonList) {
            if (auto intersect = poly.findRayIntersection(ray); intersect.has_value()) {
                double dist = calcDistance(ray.getStart(), *intersect);
                if (dist < minDist) {
                    minDist = dist;
                    bestIntersection = intersect;
                }
            }
        }
        if (bestIntersection.has_value()) {
            ray.setEnd(*bestIntersection);
        }
    }
}

void RaycasterController::filterDuplicateRays(std::vector<RaySegmentNS::RaySegment>* rays) const {
    if (rays->size() <= 1) {
        return;
    }
    auto newEnd = std::unique(
        rays->begin(), rays->end(),
        [](const RaySegmentNS::RaySegment& a, const RaySegmentNS::RaySegment& b) {
            return (calcDistance(a.getEnd(), b.getEnd()) < GlobalConfig::ENDPOINT_TOLERANCE);
        });
    rays->erase(newEnd, rays->end());
}

std::vector<QPoint> RaycasterController::computeLightArea() const {
    auto rays = generateLightRays();
    processRayIntersections(&rays);
    filterDuplicateRays(&rays);
    std::vector<QPoint> area;
    for (const auto& ray : rays) {
        area.push_back(ray.getEnd());
    }
    return area;
}
