
#include "Controller.h"

#include "labs/basics3/Polygon.h"

#include <algorithm>
#include <cmath>

// #include <iostream>

// NOLINTNEXTLINE
const int Degree = 12;

[[nodiscard]] const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    polygons_.back().AddVertex(new_vertex);
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    polygons_.back().UpdateLastVertex(new_vertex);
}

[[nodiscard]] QPointF Controller::GetLight() const {
    return light_source_;
}

void Controller::SetLight(const QPointF& light) {
    light_source_ = light;
}

std::vector<Ray> Controller::CastRays() const {
    std::vector<Ray> rays;
    for (const auto& c : polygons_) {
        for (const auto& v : c.GetVertices()) {
            rays.emplace_back(light_source_, v);
            auto r = rays.back();
            rays.push_back(r.Rotate(0.0001));

            rays.push_back(r.Rotate(-0.0001));
        }
    }
    return rays;
}

std::vector<std::vector<Ray>> Controller::CastShadow() const {
    std::vector<std::vector<Ray>> rays;
    rays.resize(Degree);
    size_t size = 0;
    for (const auto& c : polygons_) {
        size += c.GetVertices().size();
    }
    // for (const auto& c : polygons_) {
    //     for (const auto& v : c.GetVertices()) {
    //         Ray ray{QPointF(0,0), QPointF(5,0)};
    //         for (size_t i = 0; i < Degree; i++) {
    //             rays[i].emplace_back(light_source_ + ray.GetEnd(), v);
    //             auto r = rays[i].back();
    //             rays[i].push_back(r.Rotate(0.0008));
    //             rays[i].push_back(r.Rotate(-0.0008));
    //             ray = ray.Rotate(2* std::numbers::pi / Degree);
    //         }
    //     }
    // }
    Ray ray{QPointF(0, 0), QPointF(10, 0)};
    for (size_t i = 0; i < Degree; i++) {
        rays[i].reserve(3 * size);
        for (const auto& c : polygons_) {
            for (const auto& v : c.GetVertices()) {
                rays[i].emplace_back(light_source_ + ray.GetEnd(), v);
                auto r = rays[i].back();
                rays[i].push_back(r.Rotate(0.0008));
                rays[i].push_back(r.Rotate(-0.0008));
            }
        }
        ray = ray.Rotate(2 * std::numbers::pi / Degree);
    }
    return rays;
}

// NOLINTNEXTLINE
void Controller::IntersectRays(std::vector<Ray>& rays) const {
    for (auto& r : rays) {
        bool flag = true;
        for (const auto& p : polygons_) {
            auto point = p.IntersectRay(r);
            if (point.has_value()) {
                auto po = point.value();
                auto e = r.GetEnd();
                auto b = r.GetBegin();
                double d1 =
                    ((e.x() - b.x()) * (e.x() - b.x())) + ((e.y() - b.y()) * (e.y() - b.y()));
                double d2 =
                    ((po.x() - b.x()) * (po.x() - b.x())) + ((po.y() - b.y()) * (po.y() - b.y()));
                if ((d2 < d1) || flag) {
                    r.SetEnd(po);
                    flag = false;
                }
            }
        }
        if (flag) {
            r.SetEnd(r.GetBegin());
        }
    }
    std::ranges::sort(rays, [](const Ray& a, const Ray& b) {
        QPointF c = b.GetEnd() - b.GetBegin();
        QPointF d = a.GetEnd() - a.GetBegin();
        auto lx = d.x() * std::abs(d.x()) * QPointF::dotProduct(c, c);
        auto rx = c.x() * std::abs(c.x()) * QPointF::dotProduct(d, d);
        auto is_neg = [](double x) { return x < 0.0; };
        if (is_neg(c.y()) != is_neg(d.y())) {
            return is_neg(c.y());
        }
        return (lx < rx) != (is_neg(d.y()));
    });
}

void Controller::DeleteAdjacent(std::vector<Ray>& rays) {
    Ray ray(QPointF(0, 0), QPointF(0, 0));
    for (size_t i = 0; i < rays.size(); i++) {
        if (((ray.GetEnd() - rays[i].GetEnd()).manhattanLength() < 6) &&
            ((ray.GetEnd() - rays[i].GetEnd()).manhattanLength() > 0)) {
            rays.erase(rays.begin() + static_cast<int64_t>(i));
            i--;
        } else {
            ray = rays[i];
        }
    }
}

std::vector<Polygon> Controller::CreateStatic() const {
    std::vector<Polygon> result;
    result.resize(static_lights_.size(), Polygon({}));
    int i = 0;
    for (const auto& c : static_lights_) {
        std::vector<Ray> rays;
        for (const auto& e : polygons_) {
            for (const auto& v : e.GetVertices()) {
                rays.emplace_back(c, v);
                auto r = rays.back();
                rays.push_back(r.Rotate(0.0001));

                rays.push_back(r.Rotate(-0.0001));
            }
        }
        IntersectRays(rays);
        DeleteAdjacent(rays);
        for (auto& e : rays) {
            result[i].AddVertex(e.GetEnd());
        }
        i++;
    }
    return result;
}

Polygon Controller::CreateArea() const {
    auto rays = CastRays();
    IntersectRays(rays);
    DeleteAdjacent(rays);
    Polygon result({});
    for (auto& c : rays) {
        result.AddVertex(c.GetEnd());
    }
    return result;
}

std::vector<Polygon> Controller::Shadow() const {
    auto rays = CastShadow();
    std::vector<Polygon> result;
    result.resize(Degree, Polygon({}));
    for (size_t i = 0; i < Degree; i++) {
        IntersectRays(rays[i]);
        result[i].GetVertices().reserve(rays[i].size());
        DeleteAdjacent(rays[i]);
        for (auto& c : rays[i]) {
            result[i].AddVertex(c.GetEnd());
        }
    }
    // for (auto& c : rays) {
    //     result.AddVertex(c.GetEnd());
    // }
    return result;
}
