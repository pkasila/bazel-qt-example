#include "polygon.h"
#include <vector>
//#include "CrossRatio.h"

Polygon::Polygon(const std::vector<QPointF>& vertices) {
    for (const auto& p : vertices) {
        this->vertices.push_back(p);
    }
}

void Polygon::setVertices(std::vector<QPointF> v) {
    vertices.clear();
    for (const auto& el : v) {
        vertices.push_back(el);
    }
}

std::vector<QPointF> Polygon::getVertices() {
    return vertices;
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (vertices.empty()) {
        return;
    }
    vertices[vertices.size() - 1] = new_vertex;
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) {
    std::vector<std::pair<QPointF, float>> intersections;
    if (vertices.size() <= 1) {
        return std::nullopt;
    }
    for (int i = 0; i < vertices.size(); ++i) {
        std::optional<std::pair<QPointF, float>> inter = ray.intersection(vertices[i], vertices[(i + 1) % vertices.size()]);
        if (inter.has_value()) {
            intersections.push_back(inter.value());
        }
    }
    if (intersections.empty()) {
        return std::nullopt;
    }
    std::pair<QPointF, float> min = intersections[0];
    for (int i = 0; i < intersections.size(); ++i) {
        if (intersections[i].second < min.second) {
            min = intersections[i];
        }
    }
    return min.first;
}
/*
std::optional<QPointF> intersection2(QPointF a, QPointF b, QPointF c, QPointF d) {
    QPointF vec_ray = b - a;
    QPointF vec_o = d - c;

    if (std::abs(Cross(vec_ray, vec_o)) <= 1e-10) {
        return std::nullopt;
    }

    float x0 = -(a - c).x();
    float y0 = -(a - c).y();

    float x1 = vec_ray.x();
    float y1 = vec_ray.y();

    float x2 = vec_o.x();
    float y2 = vec_o.y();

    QPointF T;
    float det = Cross(QPointF(x1, -x2), QPointF(y1, -y2));
    if (std::abs(det) <= 1e-10) {
        return std::nullopt;
    } else {
        T = QPointF(1/det * (x2 * y0 - x0 * y2), 1 / det * (x1 * y0 - y1 * x0));
    }
    if (T.x() > 0 && T.y() > 0 && T.y() < 1 && T.x() < 1) {
        return a + T.x() * vec_ray;
    } else {
        return std::nullopt;
    }
}*/
/*
std::vector<Polygon> Polygon::parse() {
    std::vector<Polygon> ans;
    bool contin = true;
    if (vertices.size() < 4) return {*this};
    int start = 0;
    QPointF start_point = vertices[start];
    std::vector<std::pair<QPointF, int>> intersections;
    for (int i = 0; i < vertices.size(); ++i) {
        for (int j = 0; j < vertices.size(); ++j) {
            if (i == j) continue;
            //if ()
        }
    }
}
*/
