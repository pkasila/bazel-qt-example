#include "controller.h"

namespace raycaster {

Polygon Controller::CreateLightArea(const QPointF& origin) const {
    std::vector<Ray> rays;
    const double eps = 0.0001;
    std::vector<QPointF> targets;

    // 1. Сбор целей (вершины + самопересечения)
    std::vector<Polygon> all_obstacles = polygons_;
    all_obstacles.push_back(bounds_polygon_);

    for (const auto& poly : all_obstacles) {
        const auto& v = poly.GetVertices();
        for (const auto& p : v) targets.push_back(p);

        // Ищем самопересечения ребер
        for (size_t i = 0; i < v.size(); ++i) {
            for (size_t j = i + 1; j < v.size(); ++j) {
                auto inter = FindSegmentsIntersection(v[i], v[(i + 1) % v.size()], 
                                                      v[j], v[(j + 1) % v.size()]);
                if (inter) targets.push_back(*inter);
            }
        }
    }

    // 2. Создание лучей
    for (const auto& target : targets) {
        double angle = std::atan2(target.y() - origin.y(), target.x() - origin.x());
        for (double a : {angle, angle - eps, angle + eps}) {
            QPointF far_p(origin.x() + std::cos(a) * 2000, origin.y() + std::sin(a) * 2000);
            rays.emplace_back(origin, far_p, a);
        }
    }

    // 3. Поиск пересечений для каждого луча
    for (auto& ray : rays) {
        double min_dist = 1e9;
        for (const auto& poly : all_obstacles) {
            auto hit = poly.IntersectRay(ray);
            if (hit) {
                double d = std::hypot(hit->x() - origin.x(), hit->y() - origin.y());
                if (d < min_dist) {
                    min_dist = d;
                    ray.SetEnd(*hit);
                }
            }
        }
    }

    // 4. Сортировка по углу для правильной отрисовки
    std::sort(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) {
        return a.GetAngle() < b.GetAngle();
    });

    std::vector<QPointF> vertices;
    for (const auto& r : rays) vertices.push_back(r.GetEnd());
    return Polygon(vertices);
}

} // namespace raycaster