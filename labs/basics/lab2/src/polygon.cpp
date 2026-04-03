#include "../include/polygon.h"
#include <QPointF>
#include <cmath>

Polygon::Polygon(const QVector<QPointF>& vertices) : vertices_(vertices) {
}

Polygon::Polygon(){}

QPointF Polygon::getPoint (size_t n) const {
    if(!vertices_.empty() && n < vertices_.size()) {
        return vertices_[n];
    }
    return QPointF(0, 0);
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (!vertices_.empty()) {
        vertices_.pop_back();
        vertices_.push_back(new_vertex);
    }
}

size_t Polygon::getSize() const {
    return vertices_.size();
}

bool Polygon::isEmpty() {
    if (vertices_.size() == 0) {
        return true;
    }
    return false;
}

void Polygon::clear() {
    vertices_.clear();
}

QVector<QPointF> Polygon::getVertices() {
    return vertices_;
}
/*
std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) {
    int count = 0;
    double t1;
    double a1 = vertices_[0].rx() - vertices_[vertices_.size() - 1].rx();
    double b1 = vertices_[0].ry() - vertices_[vertices_.size() - 1].ry();
    if(a1 * ray.getB() - b1 * ray.getA() != 0) {
        double T2 = (ray.getA()*(vertices_[vertices_.size() - 1].ry()-ray.getBegin().ry()) + ray.getB()*(ray.getBegin().rx()-vertices_[vertices_.size() - 1].rx()))/(a1*ray.getB() - b1*ray.getA());
        double T1 = (vertices_[vertices_.size() - 1].rx()+a1*T2-ray.getBegin().rx())/ray.getA();
        t1 = T1;
        for (size_t i = 1; i < vertices_.size(); i++) {
            a1 = vertices_[i].rx() - vertices_[i-1].rx();
            b1 = vertices_[i].ry() - vertices_[i-1].ry();
            if(a1 * ray.getB() - b1 * ray.getA() != 0) {
                double T2 = (ray.getA()*(vertices_[i-1].ry()-ray.getBegin().ry()) + ray.getB()*(ray.getBegin().rx()-vertices_[i-1].rx()))/(a1*ray.getB() - b1*ray.getA());
                // T2 = (r_dx*(s_py-r_py) + r_dy*(r_px-s_px))/(s_dx*r_dy - s_dy*r_dx)
                double T1 = (vertices_[i-1].rx()+a1*T2-ray.getBegin().rx())/ray.getA();
                // T1 = (s_px+s_dx*T2-r_px)/r_dx
                if (std::abs(T1) < std::abs(t1)) {
                    t1 = T1;
                    count = i;
                }
            }
        }
        double XE = ray.getBegin().rx() + ray.getA() * t1;
        double YE = ray.getBegin().ry() + ray.getB() * t1;
        if (count != 0) {
            if (vertices_[count-1].rx() < vertices_[count].rx()) {
                if (vertices_[count-1].rx() < XE && vertices_[count].rx() > XE) {
                    return QPointF(XE, YE);
                }
            } else {
                if (vertices_[count-1].rx() > XE && vertices_[count].rx() < XE) {
                    return QPointF(XE, YE);
                }
            }
        } if (count == 0) {
            if (vertices_[0].rx() < vertices_[vertices_.size() - 1].rx()) {
                if (vertices_[0].rx() < XE && vertices_[vertices_.size() - 1].rx() > XE) {
                    return QPointF(XE, YE);
                }
            } else {
                if (vertices_[vertices_.size() - 1].rx() > XE && vertices_[0].rx() < XE) {
                    return QPointF(XE, YE);
                }
            }
        }
    }
    return std::nullopt;
}


std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    std::optional<QPointF> closest_intersection;
    double min_t = std::numeric_limits<double>::max();

    // Проверяем все ребра многоугольника
    for (size_t i = 0; i < vertices_.size(); i++) {
        size_t j = (i + 1) % vertices_.size();
        const QPointF& p1 = vertices_[i];
        const QPointF& p2 = vertices_[j];

        double edge_dx = p2.x() - p1.x();
        double edge_dy = p2.y() - p1.y();

        double ray_dx = ray.getA();
        double ray_dy = ray.getB();

        double denominator = edge_dx * ray_dy - edge_dy * ray_dx;
        if (std::abs(denominator) < 1e-10) {
            continue;
        }

        double t_edge = (ray_dx * (p1.y() - ray.getBegin().y()) +
                         ray_dy * (ray.getBegin().x() - p1.x()))

        double t_ray = (p1.x() + edge_dx * t_edge - ray.getBegin().x()) / ray_dx;

        // Проверяем, что пересечение в пределах ребра (t_edge ∈ [0, 1])
        // и что луч идет вперед (t_ray > 0)
        if (t_edge >= 0 && t_edge <= 1 && t_ray > 0) {
            if (t_ray < min_t) {
                min_t = t_ray;
                closest_intersection = QPointF(
                    ray.getBegin().x() + ray_dx * t_ray,
                    ray.getBegin().y() + ray_dy * t_ray
                    );
            }
        }
    }

    return closest_intersection;
}


std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    QLineF rayLine(ray.getBegin(), ray.getEnd());
    std::optional<QPointF> closestIntersection;
    double minDistance = std::numeric_limits<double>::max();

    // Проверяем пересечение с каждой стороной полигона
    for (size_t i = 0; i < vertices_.size(); ++i) {
        size_t next_i = (i + 1) % vertices_.size();
        QLineF polygonEdge(vertices_[i], vertices_[next_i]);

        QPointF intersectionPoint;
        if (rayLine.intersects(polygonEdge, &intersectionPoint) == QLineF::BoundedIntersection) {
            // Проверяем, что пересечение действительно в направлении луча
            QLineF rayToIntersection(ray.getBegin(), intersectionPoint);
            if (QLineF(ray.getBegin(), ray.getEnd()).angleTo(rayToIntersection) < 0.1) {
                double distance = QLineF(ray.getBegin(), intersectionPoint).length();
                if (distance < minDistance) {
                    minDistance = distance;
                    closestIntersection = intersectionPoint;
                }
            }
        }
    }

    return closestIntersection;
}
*/

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    const QPointF ray_begin = ray.getBegin();
    const QPointF ray_end = ray.getEnd();

    double min = std::numeric_limits<double>::max();
    QPointF closest_intersection;
    bool found = false;

    for (size_t i = 0; i < vertices_.size(); ++i) {
        const QPointF p1 = vertices_[i];
        const QPointF p2 = vertices_[(i + 1) % vertices_.size()];

        auto intersection = LineIntersection(ray_begin, ray_end, p1, p2);

        if (intersection) {
            const double dist = Distance(ray_begin, *intersection);
            if (dist < min) {
                min = dist;
                closest_intersection = *intersection;
                found = true;
            }
        }
    }
    return found ? std::optional(closest_intersection) : std::nullopt;
}

std::optional<QPointF> Polygon::LineIntersection (
    const QPointF& a1, const QPointF& a2, const QPointF& b1, const QPointF& b2) const {
    const double denom =
        (a1.x() - a2.x()) * (b1.y() - b2.y()) - (a1.y() - a2.y()) * (b1.x() - b2.x());

    if (std::abs(denom) < std::numeric_limits<double>::epsilon()) {
        return std::nullopt;
    }

    const double t =
        ((a1.x() - b1.x()) * (b1.y() - b2.y()) - (a1.y() - b1.y()) * (b1.x() - b2.x())) / denom;

    const double u =
        -((a1.x() - a2.x()) * (a1.y() - b1.y()) - (a1.y() - a2.y()) * (a1.x() - b1.x())) / denom;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        return QPointF(a1.x() + t * (a2.x() - a1.x()), a1.y() + t * (a2.y() - a1.y()));
    }
    return std::nullopt;
}
