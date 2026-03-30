#include "Polygon.h"

#include <iostream>


bool Polygon::IsIntersected(QPointF p1, QPointF p2, QPointF p3, QPointF p4, QPointF &intersection) {
    const double epsilon = 1e-10;

    double a1 = p2.y() - p1.y();
    double b1 = p1.x() - p2.x();
    double c1 = p1.y() * p2.x() - p1.x() * p2.y();

    double a2 = p4.y() - p3.y();
    double b2 = p3.x() - p4.x();
    double c2 = p3.y() * p4.x() - p3.x() * p4.y();

    double znam = a1 * b2 - a2 * b1;
    if (qAbs(znam) > epsilon) {
        // Check absolute value
        double x = -(c1 * b2 - c2 * b1) / znam;
        double y = -(a1 * c2 - a2 * c1) / znam;
        intersection = QPointF(x, y);

        // Check if intersection is within both segments
        if (x >= qMin(p1.x(), p2.x()) - epsilon && x <= qMax(p1.x(), p2.x()) + epsilon &&
            y >= qMin(p1.y(), p2.y()) - epsilon && y <= qMax(p1.y(), p2.y()) + epsilon &&
            x >= qMin(p3.x(), p4.x()) - epsilon && x <= qMax(p3.x(), p4.x()) + epsilon &&
            y >= qMin(p3.y(), p4.y()) - epsilon && y <= qMax(p3.y(), p4.y()) + epsilon) {
            return true;
        }
    }
    return false;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray &ray) {
    std::optional<QPointF> closest_intersection = std::nullopt;
    double min_distance = std::numeric_limits<double>::max();

    // Check all edges, including from last vertex back to first
    for (size_t i = 0; i < vertices_.size(); ++i) {
        size_t j = (i + 1) % vertices_.size();
        QPointF intersection;
        if (IsIntersected(vertices_[i], vertices_[j],
                          ray.GetBegin(), ray.GetEnd(), intersection)) {
            double dist = GetDistance(intersection, ray.GetBegin());
            if (dist < min_distance) {
                min_distance = dist;
                closest_intersection = intersection;
            }
        }
    }
    return closest_intersection;
}

Polygon::Polygon(const std::vector<QPointF> &vertices) {
    vertices_ = vertices;
}

std::vector<QPointF> Polygon::GetVertices() {

    return vertices_;
}

void Polygon::AddVertex(const QPointF &vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF &new_vertex) {
    auto &last_vertex = vertices_.back();
    last_vertex = new_vertex;
}

bool Polygon::IsEmpty() {
    return vertices_.empty();
}

void Polygon::Clear() {
    vertices_.clear();
}

double Polygon::GetDistance(const QPointF &p1, const QPointF &p2) {
    return qSqrt(qPow(p1.x() - p2.x(), 2) + qPow(p1.y() - p2.y(), 2));
}
