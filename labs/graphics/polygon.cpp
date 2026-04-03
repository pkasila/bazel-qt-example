#include "polygon.h"

#include <exception>
#include <stdexcept>
#include <optional>
#include <QDebug>

Polygon::Polygon(const std::vector<QPointF>& vertices) {
    this->vertices_ = vertices;
}
[[nodiscard]] int Polygon::GetVerticesNumber() const {
    return static_cast<int>(vertices_.size());
}
[[nodiscard]] QPointF Polygon::GetVertex(int index) const {
    if (index >= vertices_.size() || index < 0) {
        throw std::runtime_error{"Polygon getter index out of range"};
    }
    return vertices_[index];
}

// [[nodiscard]] QPoint& Polygon::GetVertex(int index) const {
//     if (index >= vertices_.size() || index < 0) {
//         throw std::runtime_error{"Polygon getter index out of range"};
//     }
//     return vertices_[index];
// }

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPointF& newVertex) {
    if (!vertices_.empty()) {
        vertices_.pop_back();
    }
    AddVertex(newVertex);
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    double min_T1 = 1e9;
    int sz = GetVerticesNumber();
    for (int i = 0; i <= sz; i++) {
        int index1 = i == sz ? 0 : i;
        int index2 = index1 + 1 == sz ? 0 : index1 + 1;

        Ray segment(GetVertex(index1), GetVertex(index2));

        if (segment.Dx()*ray.Dy() == segment.Dy()*ray.Dx()) { // no intersection
            continue;
        }

        double T2 = (ray.Dx()*(segment.Begin().y() - ray.Begin().y())
                    + ray.Dy()*(ray.Begin().x() - segment.Begin().x()))
                    / (segment.Dx()*ray.Dy() - segment.Dy()*ray.Dx());
        double T1;
        if (ray.Dx() > 0 || ray.Dx() < 0) {
            T1 = (segment.Begin().x() + segment.Dx()*T2 - ray.Begin().x())
                 / ray.Dx();
        } else {
            T1 = (segment.Begin().y() + segment.Dy()*T2 - ray.Begin().y())
                 / ray.Dy();
        }

        if (T1 < 1 && T1 > 0 && 0 <= T2 && T2 <= 1) {
            min_T1 = std::min(min_T1, T1);
        }
    }
    if (min_T1 < 5e8) {
        return QPointF(
            ray.Begin().x() + ray.Dx() * min_T1,
            ray.Begin().y() + ray.Dy() * min_T1
                    );
    }
    else {
        return std::nullopt;
    }
}

const QPointF* Polygon::Data() {
    return vertices_.data();
}
