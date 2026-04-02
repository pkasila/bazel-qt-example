#include "raycaster.hpp"
#include <QPainter>
#include "util.hpp"

rc::Raycaster::Raycaster(QQuickItem* parent) : QQuickPaintedItem(parent) {
    setRenderTarget(FramebufferObject);
    setPerformanceHint(FastFBOResizing, true);
    connect(this, &Raycaster::modeChanged, this, &Raycaster::finishPolygon);
    connect(this, &Raycaster::modeChanged, this, &Raycaster::redraw);
    connect(this, &Raycaster::lightPositionChanged, this, &Raycaster::redraw);
    clear();
}

void rc::Raycaster::paint(QPainter* painter) {
    QElapsedTimer timer;
    timer.start();

    drawPolygons(painter);
    drawLights(painter);

    frameTime = timer.nsecsElapsed() / 1000.0;
    int fps = 1e6 / frameTime;
    if(fps > 10000) {
        fpsText = "∞ FPS";
    } else {
        fpsText = QString::asprintf("%i FPS", fps);
    }
    fpsTextChanged();
}

void rc::Raycaster::redraw() { update(); }

void rc::Raycaster::drawPolygons(QPainter* painter) {
    painter->setPen(QPen(QColor("#eeeeee"), 2));
    painter->setBrush(QColor("#eeeeee"));
    painter->setRenderHint(QPainter::Antialiasing, true);

    for(int p = 1; p < polygons.size(); p++) {
        for(int i = 0; i < polygons[p].size(); i++) {
            QPointF cur = cam.toLocal(polygons[p].at(i), height());
            QPointF next = cam.toLocal(polygons[p].at((i + 1) % polygons[p].size()), height());
            if(mode == 1) {
                painter->drawEllipse(cur.x() - 4, cur.y() - 4, 8, 8);
            }
            painter->drawLine(cur.x(), cur.y(), next.x(), next.y());
        }
    }
}

void rc::Raycaster::drawLights(QPainter* painter) {
    for(const QPointF& source : staticLights) {
        drawLightSource(painter, source);
    }
    if(mode == 2) {
        drawLightSource(painter, cam.toGlobal(lightPosition, height()));
    }
}

void rc::Raycaster::drawLightSource(QPainter* painter, QPointF source) {
    std::vector<QPointF> polygon = getLightPolygon(source);
    painter->setPen(QPen(QColor("#00000000")));
    painter->setBrush(QColor("#2eeeeeee"));
    painter->drawPolygon(polygon.data(), polygon.size());

    for(double angle = 0; angle < std::numbers::pi * 2; angle += std::numbers::pi / 3) {
        QPointF pos = source + QPointF(std::cos(angle), std::sin(angle)) * 0.75;
        std::vector<QPointF> polygon = getLightPolygon(pos);
        painter->drawPolygon(polygon.data(), polygon.size());
    }
}

std::vector<QPointF> rc::Raycaster::getLightPolygon(const QPointF& source) {
    std::vector<Ray> rays;
    for(const Polygon& polygon : polygons) {
        for(int i = 0; i < polygon.size(); i++) {
            Ray ray(source, polygon.at(i));
            rays.push_back(ray);
            rays.push_back(ray.rotate(0.01));
            rays.push_back(ray.rotate(-0.01));
        }
    }
    for(QPointF p : extraPoints) {
        Ray ray(source, p);
        rays.push_back(ray);
        rays.push_back(ray.rotate(0.01));
        rays.push_back(ray.rotate(-0.01));
    }

    std::ranges::sort(rays, [](const Ray& a, const Ray& b) { return a.getAngle() < b.getAngle(); });

    std::vector<QPointF> raw;
    for(const Ray& ray : rays) {
        std::optional<QPointF> pos;
        for(const Polygon& polygon : polygons) {
            std::optional<QPointF> cur = polygon.intersect(ray);
            if(!cur.has_value()) {
                continue;
            }
            if(!pos.has_value() || distance(source, cur.value()) < distance(source, pos.value())) {
                pos = cur;
            }
        }
        if(!pos.has_value()) {
            pos = (QVector2D(ray.getOrigin()) + ray.getDirection() * 1000000).toPointF();
        }
        if(pos.has_value()) {
            raw.push_back(pos.value());
        }
    }

    std::vector<QPointF> res;
    for(QPointF point : raw) {
        point = cam.toLocal(point, height());
        if(res.empty() || distance(point, res.back()) > 0.1) {
            res.push_back(point);
        }
    }

    return res;
}

void rc::Raycaster::rescale(double factor, QPointF mouse) {
    cam.rescale(factor, mouse, size());
    update();
}

void rc::Raycaster::shift(QPointF mouse) {
    if(!shiftActive) {
        shiftActive = true;
    } else {
        QPointF delta = mouse - lastMouse;
        cam.shift(delta, height());
    }
    lastMouse = mouse;
    update();
}

void rc::Raycaster::resetShift() { shiftActive = false; }

void rc::Raycaster::newVertex(QPointF point) {
    point = cam.toGlobal(point, height());
    if(polygons.back().size() == 0) {
        polygons.back().add(point);
    }
    polygons.back().add(point);
    rebuildExtraPoints();
    update();
}

void rc::Raycaster::setLastVertex(QPointF point) {
    point = cam.toGlobal(point, height());
    if(polygons.back().size() == 0) {
        return;
    }
    polygons.back().setLast(point);
    rebuildExtraPoints();
    update();
}

void rc::Raycaster::finishPolygon() {
    if(polygons.size() >= 2 && polygons.back().size() != 0) {
        polygons.back().pop();
        polygons.emplace_back();
        rebuildExtraPoints();
        update();
    }
}

void rc::Raycaster::rebuildExtraPoints() {
    extraPoints.clear();
    for(int p1 = 0; p1 < polygons.size(); p1++) {
        for(int p2 = p1; p2 < polygons.size(); p2++) {
            for(int i = 0; i < polygons[p1].size(); i++) {
                for(int j = 0; j < polygons[p2].size(); j++) {
                    QPointF a = polygons[p1].at(i);
                    QPointF b = polygons[p1].at((i + 1) % polygons[p1].size());
                    QPointF c = polygons[p2].at(j);
                    QPointF d = polygons[p2].at((j + 1) % polygons[p2].size());
                    std::optional<QPointF> p = intersectSegments(a, b, c, d);
                    if(p.has_value()) {
                        extraPoints.push_back(p.value());
                    }
                }
            }
        }
    }
}

void rc::Raycaster::addStaticLight() { staticLights.push_back(cam.toGlobal(lightPosition, height())); }

void rc::Raycaster::clear() {
    Polygon border;
    border.add({-2e4, -2e4});
    border.add({2e4, -2e4});
    border.add({2e4, 2e4});
    border.add({-2e4, 2e4});
    polygons.clear();
    polygons.push_back(border);
    polygons.emplace_back();
    staticLights.clear();
    cam = Camera();
    update();
}
