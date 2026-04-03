#include "../include/controller.h"

Controller::Controller() {
    light_source_ = QPointF(0,0);
    for (int i = 0; i < 9; i++) {
        light_sources.push_back(QPointF(0,0));
    }
}

const QVector<Polygon>& Controller::GetPolygons() {
    return polygons;
}

void Controller::AddPolygon(const Polygon& p) {
    polygons.push_back(p);
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (!polygons.empty()) {
        polygons[polygons.size() - 1].AddVertex(new_vertex);
    }
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!polygons.empty()) {
        polygons[polygons.size() - 1].UpdateLastVertex(new_vertex);
    }
}

QPointF Controller::getLightSource() {
    return light_source_;
}

QPointF Controller::getLightSource2() {
    return light_source_2;
}

void Controller::setLightSource(QPointF point) {
    light_source_ = point;
}

void Controller::setLightSource2(QPointF point) {
    light_source_2 = point;
}

Polygon Controller::getLastPolygon() {
    return polygons[polygons.size() - 1];
}

bool Controller::isEmpty() {
    if (polygons.size() > 0) {
        return false;
    }
    return true;
}

void Controller::clear() {
    polygons.clear();
}

size_t Controller::size() {
    return polygons.size();
}

QVector<QPointF> Controller::getPolygon(size_t n) {
    if (!polygons.empty() && n < polygons.size()) {
        return polygons[n].getVertices();
    }
    return Polygon().getVertices();
}

std::vector<Ray> Controller::CastRays(QPointF ls) {
    std::vector<Ray> rays;
    const double epsilon = 0.0001;
    const double max_ray_length = 1000.0;

    auto createRay = [&](double angle) {
        QPointF direction(
            ls.x() + max_ray_length * std::cos(angle),
            ls.y() + max_ray_length * std::sin(angle)
            );

        Ray ray(ls, direction, angle);

        QPointF closest_intersection;
        double min_distance = std::numeric_limits<double>::max();
        bool has_intersection = false;

        for (const auto& polygon : polygons) {
            if (auto intersection = polygon.IntersectRay(ray)) {
                double dist = QLineF(ls, *intersection).length();
                if (dist < min_distance) {
                    min_distance = dist;
                    closest_intersection = *intersection;
                    has_intersection = true;
                }
            }
        }

        if (has_intersection) {
            ray.setEnd(closest_intersection);
        }

        return ray;
    };

    for (const auto& polygon : polygons) {
        for (size_t j = 0; j < polygon.getSize(); ++j) {
            const QPointF& vertex = polygon.getPoint(j);
            double main_angle = std::atan2(vertex.y() - ls.y(),
                                           vertex.x() - ls.x());
            rays.push_back(createRay(main_angle));
            rays.push_back(createRay(main_angle + epsilon));
            rays.push_back(createRay(main_angle - epsilon));
        }
    }

    return rays;
}

void Controller::createOffsetRay(std::vector<Ray>& rays, const Polygon& polygon,
                                 const QPointF& vertex, double base_angle, double angle_offset) {
    double new_angle = base_angle + angle_offset;

    const double ray_length = 1000.0;
    QPointF dir_point(
        light_source_.x() + ray_length * std::cos(new_angle),
        light_source_.y() + ray_length * std::sin(new_angle)
        );

    Ray offset_ray(light_source_, dir_point, new_angle);
    if (auto intersection = polygon.IntersectRay(offset_ray)) {
        offset_ray.setEnd(*intersection);
    }
    rays.push_back(std::move(offset_ray));
}
void Controller::IntersectRays(std::vector<Ray>* rays) {
    for (auto& ray : *rays) {
        double min_dist = Distance(ray.getBegin(), ray.getEnd());
        QPointF closest_intersection = ray.getEnd();

        for (auto& polygon : polygons) {
            auto intersection = polygon.IntersectRay(ray);
            if (intersection) {
                const double dist = Distance(ray.getBegin(), *intersection);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_intersection = *intersection;
                }
            }
        }

        ray.setEnd(closest_intersection);
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) {
    // Сортируем лучи
    std::sort(rays->begin(), rays->end(),
              [](const Ray& a, const Ray& b) {
                  return a.getAngle() < b.getAngle();
              });
    // Удаляем лучи из вектора
    std::vector<Ray> new_rays;
    new_rays.push_back(rays->front());

    for (size_t i = 1; i < rays->size(); ++i) {
        const Ray& prev = new_rays.back();
        const Ray& current = (*rays)[i];
        if (std::abs(Distance(current.getEnd(), prev.getEnd()) > 1.0)) {
            new_rays.push_back(current);
        }
    }
    *rays = std::move(new_rays);
}

Polygon Controller::CreateLightArea(QPointF ls) {
    std::vector<Ray> rays = CastRays(ls);
    if (rays.empty()) return Polygon();
    std::sort(rays.begin(), rays.end(), [this](const Ray& a, const Ray& b) {
        return a.getAngle() < b.getAngle();
    });
    Polygon lightArea;
    lightArea.AddVertex(ls);

    for (const auto& ray : rays) {
        lightArea.AddVertex(ray.getEnd());
    }
    lightArea.AddVertex(rays.front().getEnd());
    return lightArea;
}

void Controller::DrawLightArea(QPainter& painter, size_t n) {
    if (n < light_sources.size()) {
        Polygon area = CreateLightArea(light_sources[n]);

        if (area.getSize() > 2) {
                QPolygonF qpolygon;
            qpolygon.reserve(area.getSize());

            for (size_t i = 0; i < area.getSize(); ++i) {
                    qpolygon.append(area.getPoint(i));
            }

            QColor color(255,255 ,255 ,40);
            painter.setBrush(color);
            painter.setPen(Qt::NoPen);
            painter.drawPolygon(qpolygon);
        }
    }
}

QPointF Controller::getvls (size_t n) {
    if (n < light_sources.size()) {
        return light_sources[n];
    }
    return QPointF(0,0);
}

void Controller::setvls (QPointF p, size_t n) {
    if (n < light_sources.size()) {
        light_sources[n] = p;
    }
}
