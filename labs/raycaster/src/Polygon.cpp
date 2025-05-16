#include "Polygon.h"
#include <cmath>
#include <climits>

Polygon::Polygon() {
    vertices.reserve(20);
}

Polygon::Polygon(const std::vector<QPointF>& vertices) : vertices(vertices) {
    if (this->vertices.capacity() < 20) {
        this->vertices.reserve(20);
    }
}
std::vector<QPointF> Polygon::GetVertices() const {
    return vertices;
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (!vertices.empty()) {
        vertices.pop_back();
    }
    vertices.push_back(new_vertex);
}

void Polygon::Complete() {
    completed = true;
}

bool Polygon::IsComplete() const {
    return completed;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    std::optional<QPointF> result;
    double minlen = std::numeric_limits<double>::max();

    for (int i = 1; i < static_cast<int>(vertices.size()); ++i) {
        QPointF p1 = vertices[i - 1];
        QPointF p2 = vertices[i];

        auto intersection = IntersectLine(p1, p2, ray);
        if (intersection) {
            double len = std::hypot(intersection->y() - ray.GetBegin().y(),
                                    intersection->x() - ray.GetBegin().x());
            if (len < minlen) {
                minlen = len;
                result = intersection;
            }
        }
    }

    if (!vertices.empty()) {
        QPointF p1 = vertices.back();
        QPointF p2 = vertices.front();
        auto intersection = IntersectLine(p1, p2, ray);
        if (intersection) {
            double len = std::hypot(intersection->y() - ray.GetBegin().y(),
                                    intersection->x() - ray.GetBegin().x());
            if (len < minlen) {
                minlen = len;
                result = intersection;
            }
        }
    }

    return result;
}

std::optional<QPointF> Polygon::IntersectLine(const QPointF& p1, const QPointF& p2, const Ray& ray) const {
    QPointF r_origin = ray.GetBegin();
    QPointF r_dir = ray.GetEnd() - ray.GetBegin();

    QPointF s_origin = p1;
    QPointF s_dir = p2 - p1;

    double r_dx = r_dir.x();
    double r_dy = r_dir.y();
    double s_dx = s_dir.x();
    double s_dy = s_dir.y();

    double denom = s_dx * r_dy - s_dy * r_dx;

    if (std::abs(denom) < 1e-9) {
        return std::nullopt;
    }

    double dx = s_origin.x() - r_origin.x();
    double dy = s_origin.y() - r_origin.y();

    double t2 = (r_dx * dy + r_dy * (-dx)) / denom;
    double t1 = (s_origin.x() + s_dx * t2 - r_origin.x()) / (r_dx != 0 ? r_dx : 1e-9); // защита от деления на 0

    if (t1 < 0 || t2 < 0 || t2 > 1) {
        return std::nullopt;
    }

    QPointF intersection = r_origin + t1 * r_dir;
    return intersection;
}
