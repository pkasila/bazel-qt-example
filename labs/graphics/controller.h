#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "raycaster.h"

#include <QColor>
#include <vector>

struct StaticLight {
    QPointF pos;
    QColor color;
};

class Controller {
   public:
    const std::vector<Polygon>& GetPolygons() const {
        return m_polygons;
    }

    void AddPolygon(const Polygon& p) {
        m_polygons.push_back(p);
    }

    void AddVertexToLastPolygon(const QPointF& v) {
        if (!m_polygons.empty()) {
            m_polygons.back().AddVertex(v);
        }
    }

    void UpdateLastPolygon(const QPointF& v) {
        if (!m_polygons.empty()) {
            m_polygons.back().UpdateLastVertex(v);
        }
    }

    void ClearBoundary() {
        if (!m_polygons.empty()) {
            m_polygons.erase(m_polygons.begin());
        }
    }

    void SetBoundary(const Polygon& p) {
        if (m_polygons.empty()) {
            m_polygons.push_back(p);
        } else {
            m_polygons[0] = p;
        }
    }

    void SetLightSource(const QPointF& pos) {
        m_baseSource = pos;
        UpdateLightCluster();
    }

    QPointF GetBaseLightSource() const {
        return m_baseSource;
    }

    const std::vector<QPointF>& GetLightCluster() const {
        return m_lightCluster;
    }

    void AddStaticLight(const QPointF& pos, QColor color) {
        if (!IsPointInAnyPolygon(pos)) {
            m_staticLights.push_back({pos, color});
        }
    }

    const std::vector<StaticLight>& GetStaticLights() const {
        return m_staticLights;
    }

    bool IsPointInAnyPolygon(const QPointF& p) const {
        for (size_t i = 1; i < m_polygons.size(); ++i) {
            if (m_polygons[i].IsPointInside(p)) {
                return true;
            }
        }
        return false;
    }

    std::vector<Ray> CastRays(const QPointF& source);
    void IntersectRays(std::vector<Ray>* rays);
    void RemoveAdjacentRays(std::vector<Ray>* rays);
    Polygon CreateLightArea(const QPointF& source);

   private:
    void UpdateLightCluster() {
        m_lightCluster = {
          m_baseSource, m_baseSource + QPointF(2, 2), m_baseSource + QPointF(-2, -2),
          m_baseSource + QPointF(2, -2), m_baseSource + QPointF(-2, 2)};
    }

    std::vector<Polygon> m_polygons;
    std::vector<StaticLight> m_staticLights;
    QPointF m_baseSource;
    std::vector<QPointF> m_lightCluster;
};

#endif