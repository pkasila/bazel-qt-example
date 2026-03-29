//
// Created by Zazerg on 3/31/25.
//

#include "Polygon.h"

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& vertex) {
    vertices_[vertices_.size() - 1] = vertex;
}

std::optional<QPointF> Polygon::Intersect(Ray& ray) const {
    std::optional<QPointF> result;
    for (int i = 0; i < vertices_.size(); i++) {
        if (result = ray.Intersect(Ray(vertices_[i], vertices_[(i+1) % vertices_.size()], true)); result != std::nullopt) {
            ray.setDirection(result.value());
            ray.setSegment(true);
        }
    }
    return ray.direction();
}

const std::vector<QPointF>& Polygon::getVertices() const {
    return vertices_;
}

int Polygon::length () const {
    return vertices_.size();
}

bool Polygon::isPointInside(const QPointF& point) const {
    Ray r = Ray(point, point + QPointF(1, 0), false);
    int ic = 0;
    bool last = false;
    for (int i = 0; i < vertices_.size(); i++) {
        if (last) {
            last = false;
            continue;
        }
        if(r.Intersect(Ray(vertices_[i], vertices_[(i + 1) % vertices_.size()], true)).has_value()) {
            last = true;
            ++ic;
        }
    }
    return ic % 2;
}
