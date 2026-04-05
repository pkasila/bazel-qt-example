#ifndef MATH_H
#define MATH_H

#include <QtCore/QPointF>
#include <cmath>
#include <limits>
#include <optional>
#include <vector>

namespace raycaster {

class Ray {
   public:
    Ray(const QPointF& begin, const QPointF& end, double angle) : begin_(begin), angle_(angle) {
        double dx = end.x() - begin.x();
        double dy = end.y() - begin.y();
        length_ = std::sqrt(dx * dx + dy * dy);
    }

    QPointF GetBegin() const {
        return begin_;
    }

    void SetBegin(const QPointF& begin) {
        begin_ = begin;
    }

    QPointF GetEnd() const {
        return QPointF(
            begin_.x() + std::cos(angle_) * length_, begin_.y() + std::sin(angle_) * length_);
    }

    void SetEnd(const QPointF& end) {
        double dx = end.x() - begin_.x();
        double dy = end.y() - begin_.y();
        length_ = std::sqrt(dx * dx + dy * dy);
    }

    double GetAngle() const {
        return angle_;
    }

    void SetAngle(double angle) {
        angle_ = angle;
    }

    double GetLength() const {
        return length_;
    }

    void SetLength(double len) {
        length_ = len;
    }

    Ray Rotate(double relative_angle) const {
        return Ray(begin_, GetEnd(), angle_ + relative_angle);
    }

   private:
    QPointF begin_;
    double angle_;
    double length_;
};

class Polygon {
   public:
    explicit Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {
    }

    Polygon() = default;

    const std::vector<QPointF>& GetVertices() const {
        return vertices_;
    }

    void AddVertex(const QPointF& vertex) {
        vertices_.push_back(vertex);
    }

    void UpdateLastVertex(const QPointF& new_vertex) {
        if (!vertices_.empty()) {
            vertices_.back() = new_vertex;
        }
    }

    /**
     * Ищет ближайшее пересечение луча с ребрами многоугольника.
     */
    std::optional<QPointF> IntersectRay(const Ray& ray) const {
        if (vertices_.size() < 2) {
            return std::nullopt;
        }

        std::optional<QPointF> closest_point = std::nullopt;
        double min_t = std::numeric_limits<double>::max();

        QPointF O = ray.GetBegin();
        double dx = std::cos(ray.GetAngle());
        double dy = std::sin(ray.GetAngle());

        // Проходим по всем ребрам многоугольника
        for (size_t i = 0; i < vertices_.size(); ++i) {
            QPointF A = vertices_[i];
            // Замыкаем многоугольник: последнее ребро соединяет последнюю и первую вершины
            QPointF B = vertices_[(i + 1) % vertices_.size()];

            QPointF V = B - A;
            QPointF W = A - O;

            // Знаменатель (векторное произведение направления луча и вектора ребра)
            double denom = V.x() * dy - V.y() * dx;

            if (std::abs(denom) < 1e-9) {
                continue;  // Параллельны
            }

            double t = (V.x() * W.y() - V.y() * W.x()) / denom;
            double u = (dx * W.y() - dy * W.x()) / denom;

            // t > 0: пересечение впереди луча
            // 0 <= u <= 1: пересечение именно в границах отрезка AB
            if (t > 1e-9 && u >= 0.0 && u <= 1.0) {
                if (t < min_t) {
                    min_t = t;
                    closest_point = QPointF(O.x() + t * dx, O.y() + t * dy);
                }
            }
        }

        return closest_point;
    }

   private:
    std::vector<QPointF> vertices_;
};

// В math.h или прямо в controller.cpp
inline std::optional<QPointF> FindSegmentsIntersection(QPointF A, QPointF B, QPointF C, QPointF D) {
    QPointF V = B - A;
    QPointF W = D - C;

    double denom = V.x() * W.y() - V.y() * W.x();
    if (std::abs(denom) < 1e-9) {
        return std::nullopt;  // Параллельны
    }

    QPointF AC = C - A;
    double t = (AC.x() * W.y() - AC.y() * W.x()) / denom;
    double u = (AC.x() * V.y() - AC.y() * V.x()) / denom;

    if (t > 1e-9 && t < 1.0 - 1e-9 && u > 1e-9 && u < 1.0 - 1e-9) {
        return QPointF(A.x() + t * V.x(), A.y() + t * V.y());
    }
    return std::nullopt;
}

}  // namespace raycaster

#endif  // MATH_H