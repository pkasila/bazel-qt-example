//
// Created by Zazerg on 3/31/25.
//

#include "Controller.h"

const std::vector<Polygon>& Controller::getPolygons() const {
    return polygons_;
}

void Controller::addPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
}

void Controller::addPolygon() {
    for (auto p : c_ints_) {
        ints_.push_back(p);
    }
    c_ints_.clear();
    addPolygon(Polygon());
}


void Controller::updateLastPolygon(const QPointF& point) {
    polygons_.back().UpdateLastVertex(point);
}

void Controller::addVertexToLastPolygon(const QPointF& point) {
    c_ints_.clear();
    if (polygons_.back().getVertices().size() == 0) {
        polygons_.back().AddVertex(point);
        return;
    }
    Ray r = Ray(polygons_.back().getVertices().back(), point, true);
    Ray r1 = Ray(polygons_.back().getVertices().front(), point, true);
    polygons_.back().AddVertex(point);
    for (int j = 0; j < polygons_.size(); ++j) {
        auto& v = polygons_[j].getVertices();
        for (int i = 0; i < v.size() - 2 * (j == polygons_.size() - 1); ++i) {
            auto i1 = r.Intersect(Ray(v[i], v[(i+1) % v.size()], true));
            if (i1.has_value()) {
                ints_.push_back(i1.value());
            }
            i1 = r1.Intersect(Ray(v[i], v[(i+1) % v.size()], true));
            if (i1.has_value()) {
                c_ints_.push_back(i1.value());
            }
        }
    }

}

const LightSource& Controller::getLightSource() {
    return lights_[0];
}

void Controller::setLightSource(QPointF point, int w, int h) {
    if (point.x() > w-6) point.setX(w-6);
    if (point.y() > h-6) point.setY(h-6);
    if (point.x() < 6) point.setX(6);
    if (point.y() < 6) point.setY(6);
    lights_[0].moveTo(point);
}

std::vector<Ray> Controller::castRays(QPointF lightSource, int w, int h) {
    std::vector<Ray> rays;
    Polygon outer = Polygon({{0, 0}, {qreal(w), 0}, {qreal(w), qreal(h)}, {0, qreal(h)}});
    polygons_.push_back(outer);
    for (auto& polygon : polygons_) {
        if (polygon.length() < 2) continue;
        for (auto& vertex: polygon.getVertices()) {
            Ray ray = Ray(lightSource, vertex, true);
            rays.push_back(ray);
            rays.push_back(ray.rotate(10000, 1));
            rays.push_back(ray.rotate(10000, -1));
        }
    }
    for (auto& c : c_ints_) {
        Ray ray = Ray(lightSource, c, true);
        rays.push_back(ray);
        rays.push_back(ray.rotate(10000, 1));
        rays.push_back(ray.rotate(10000, -1));
    }
    for (auto& c : ints_) {
        Ray ray = Ray(lightSource, c, true);
        rays.push_back(ray);
        rays.push_back(ray.rotate(10000, 1));
        rays.push_back(ray.rotate(10000, -1));
    }
    polygons_.pop_back();
    return rays;
}

void Controller::intersectRays(std::vector<Ray>& rays, int w, int h) {
    Polygon outer = Polygon({{0, 0}, {qreal(w), 0}, {qreal(w), qreal(h)}, {0, qreal(h)}});
    polygons_.push_back(outer);
    for (auto& ray: rays) {
        for (auto& polygon: polygons_) {
            polygon.Intersect(ray);
        }
    }
    polygons_.pop_back();
}


std::vector<Polygon> Controller::createHalfShades(QPointF lightSource, int w, int h) {
    std::vector<Polygon> half_shades;
    Polygon outer = Polygon({{0, 0}, {qreal(w), 0}, {qreal(w), qreal(h)}, {0, qreal(h)}});
    Ray light = Ray(lightSource, lightSource + QPointF(5,0), false);
    for (int i = 0; i < 8; i++) {
        Ray l1 = Ray(lightSource, light.direction(), true);
        polygons_.push_back(outer);
        for (auto& polygon: polygons_) {
            polygon.Intersect(l1);
        }
        polygons_.pop_back();
        if (l1.direction() == light.direction()) {
            half_shades.push_back(fromLightSource(light.direction(), w, h));
        }
        light = light.rotate(std::numbers::sqrt2/2, std::numbers::sqrt2/2);
    }
    return half_shades;
}

Polygon Controller::fromLightSource(QPointF lightSource, int w, int h) {
    Polygon lightArea = Polygon();
    std::vector<Ray> rays = castRays(lightSource, w, h);
    intersectRays(rays, w, h);
    std::ranges::sort(rays, [] (const Ray& a, const Ray& b) {
        QPointF d1 = a.direction() - a.origin();
        QPointF d2 = b.direction() - b.origin();
        if ((d1.y() < 0.0) != (d2.y() < 0.0)) return d2.y() < 0.0;
        return d1.x() * d2.y() > d2.x() * d1.y() || (d2.y() == 0.0 && d1.y() == 0.0 && d2.x() < 0 && d1.x() > 0);
    });

    for (int i = 0; i < rays.size(); ++i) {
        // if ((rays[i].direction() - rays[(i+1) % rays.size()].direction()).manhattanLength() < 4) {continue;}
        lightArea.AddVertex(rays[i].direction());
    }
    return lightArea;
}

std::vector<LightSource>& Controller::getLightSources() {
    return lights_;
}

void Controller::addLightSource(LightSource light_source) {
    lights_.push_back(light_source);
}

