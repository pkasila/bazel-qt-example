#pragma once

#include "../input/polygon.h"
#include "../input/ray.h"
#include <QPointF>
#include <optional>
#include <set>
#include <vector>

struct QPointFCompare {
    static constexpr double epsilon = 1e-7;
    bool operator()(const QPointF& a, const QPointF& b) const {
        if (std::abs(a.x() - b.x()) > epsilon) return a.x() < b.x();
        if (std::abs(a.y() - b.y()) > epsilon) return a.y() < b.y();
        return false;
    }
};

class Controller {
public:
    Controller();

    const std::vector<Polygon>& getPolygons() const { return polygons_; }
    const std::optional<Polygon>& getCurrentPolygon() const { return current_polygon_; }
    bool addPolygon(Polygon&& polygon);
    void startNewPolygon(const QPointF& first_vertex);
    void addVertexToCurrentPolygon(const QPointF& vertex);
    void updateCurrentPolygonLastVertex(const QPointF& new_vertex);
    bool finalizeCurrentPolygon();
    void addBoundaryPolygon(qreal minX, qreal minY, qreal maxX, qreal maxY);

    const std::vector<QPointF>& getLightSources() const { return light_sources_; }
    QPointF getLightSourceCenter() const { return light_source_center_; }
    bool setLightSourceCenter(const QPointF& center);

    const std::vector<QPointF>& getStaticLightSources() const { return static_light_sources_; }
    bool addStaticLightSource(const QPointF& pos);

    bool removePolygon(size_t index);
    bool removeStaticLightSource(size_t index);
    size_t getPolygonCount() const;
    size_t getStaticLightSourceCount() const { return static_light_sources_.size(); };

    std::vector<Ray> castRays(const QPointF& source) const;
    void intersectRays(std::vector<Ray>* rays) const;
    void removeAdjacentRays(std::vector<Ray>* rays) const;

    std::vector<Polygon> createDynamicLightAreas() const;
    std::vector<Polygon> createStaticLightAreas() const;

private:
    std::vector<Polygon> polygons_;
    std::optional<Polygon> current_polygon_;

    QPointF light_source_center_{0.0, 0.0};
    std::vector<QPointF> light_sources_;
    static constexpr int num_light_sources_ = 5;
    static constexpr double light_source_spread_ = 6.0;

    std::vector<QPointF> static_light_sources_;

    void regenerateLightSources();

    bool isPointInsideAnyPolygon(const QPointF& point, bool includeBoundary = false) const;
    bool isBoundary(const Polygon& poly) const;
    bool isBoundaryIndex(size_t index) const;
    bool isLightSourceInside(const Polygon& poly) const;

    std::vector<Polygon> createLightAreasForSources(const std::vector<QPointF>& sources) const;
    std::set<QPointF, QPointFCompare> findAllSelfIntersectionPoints() const;
    std::optional<QPointF> intersectSegments(const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4) const;

    static constexpr double RAY_ANGLE_OFFSET = 0.0001;
    static constexpr double ADJACENT_RAY_DIST_THRESHOLD_SQ = 0.5 * 0.5;
    static constexpr double MAX_RAY_LENGTH = 5000.0;
};