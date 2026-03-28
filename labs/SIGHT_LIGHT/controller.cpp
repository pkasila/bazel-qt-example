#include "controller.h"
#include <algorithm>
#include <cmath>
#include <QRect>

Controller::Controller(int width, int height)
    : m_width(width), m_height(height)
{
    initializeBoundingPolygon();
}

void Controller::initializeBoundingPolygon()
{
    std::vector<QPointF> vertices = {
        QPointF(1, 1),
        QPointF(799, 1),
        QPointF(799, 599),
        QPointF(1, 599)
    };
    m_bounding_polygon = Polygon(vertices);
}

const std::vector<Polygon>& Controller::getPolygons() const
{
    return m_polygons;
}

void Controller::addPolygon(const Polygon& polygon)
{
    m_polygons.push_back(polygon);
}

void Controller::addVertexToLastPolygon(const QPointF& new_vertex)
{
    if (!m_polygons.empty()) {
        m_polygons.back().addVertex(new_vertex);
    }
}

void Controller::updateLastPolygon(const QPointF& new_vertex)
{
    if (!m_polygons.empty()) {
        m_polygons.back().updateLastVertex(new_vertex);
    }
}

QPointF Controller::getLightSource() const
{
    return m_light_source;
}

void Controller::setLightSource(const QPointF& light_source)
{
    m_light_source = light_source;
}

std::vector<Ray> Controller::castRays() const
{
    std::vector<Ray> rays;

    for (const auto& polygon : m_polygons) {
        for (const auto& vertex : polygon.getVertices()) {
            Ray main_ray(m_light_source, vertex);
            rays.push_back(main_ray);
            rays.push_back(main_ray.rotate(-0.0001));
            rays.push_back(main_ray.rotate(0.0001));

         // rays.push_back(main_ray.rotate(-0.2));
           //  rays.push_back(main_ray.rotate(0.2));
        }
    }

    //int i = 0;
    for (const auto& vertex : m_bounding_polygon.getVertices()) {
        //i++;
        Ray main_ray(m_light_source, vertex);
        rays.push_back(main_ray);
       for (double i = 0.1; i < 6; i+=0.1){
        rays.push_back(main_ray.rotate(-i));
        //rays.push_back(main_ray.rotate(-0.0002));
      // rays.push_back(main_ray.rotate(0.0002));
        rays.push_back(main_ray.rotate(i));
        }
       for (double i = 0.0001; i < 0.0006; i+=0.0001){
           rays.push_back(main_ray.rotate(-i));
           //rays.push_back(main_ray.rotate(-0.0002));
           // rays.push_back(main_ray.rotate(0.0002));
           rays.push_back(main_ray.rotate(i));
       }
       // if ( i == 3) break;
    }

    return rays;
}

std::vector<Ray> Controller::castRaysSources(const QPointF& source) const
{
    std::vector<Ray> rays;

    for (const auto& polygon : m_polygons) {
        for (const auto& vertex : polygon.getVertices()) {
            Ray main_ray(source, vertex);
            rays.push_back(main_ray);
            rays.push_back(main_ray.rotate(-0.0001));
            rays.push_back(main_ray.rotate(0.0001));

            // rays.push_back(main_ray.rotate(-0.2));
            //  rays.push_back(main_ray.rotate(0.2));
        }
    }

    //int i = 0;
    for (const auto& vertex : m_bounding_polygon.getVertices()) {
        //i++;
        Ray main_ray(source, vertex);
        rays.push_back(main_ray);
        for (double i = 0.5; i < 6; i+=0.5){
             rays.push_back(main_ray.rotate(-i));
        //     //rays.push_back(main_ray.rotate(-0.0002));
        //     // rays.push_back(main_ray.rotate(0.0002));
             rays.push_back(main_ray.rotate(i));
         }
        for (double i = 0.0001; i < 0.0006; i+=0.0001){
            rays.push_back(main_ray.rotate(-i));
            //rays.push_back(main_ray.rotate(-0.0002));
            // rays.push_back(main_ray.rotate(0.0002));
            rays.push_back(main_ray.rotate(i));
        }
        // if ( i == 3) break;
    }

    return rays;
}

bool pointOnRay(const QPointF& p, const QPoint& ray_begin, const QPointF& ray_end)
{
    int cross = (p.x() - ray_begin.x()) * (ray_end.y() - ray_begin.y()) -
                (p.y() - ray_begin.y()) * (ray_end.x() - ray_begin.x());
    if (abs(cross) > 1e-8) return false;

    bool x_ok = (ray_end.x() >= ray_begin.x()) ?
                    (p.x() >= ray_begin.x()) : (p.x() <= ray_begin.x());
    bool y_ok = (ray_end.y() >= ray_begin.y()) ?
                    (p.y() >= ray_begin.y()) : (p.y() <= ray_begin.y());

    return x_ok && y_ok;
}

std::optional<QPointF> Controller::lineIntersection(const QPointF& p1, const QPointF& p2,
                                                const QPointF& p3, const QPointF& p4) const
{
    double a1 = p2.y() - p1.y();
    double b1 = p1.x() - p2.x();
    double c1 = a1 * p1.x() + b1 * p1.y();

    double a2 = p4.y() - p3.y();
    double b2 = p3.x() - p4.x();
    double c2 = a2 * p3.x() + b2 * p3.y();

    double determinant = a1 * b2 - a2 * b1;

    if (determinant == 0) {
        return std::nullopt;
    }

    double x = (b2 * c1 - b1 * c2) / determinant;
    double y = (a1 * c2 - a2 * c1) / determinant;

    QPointF intersection(static_cast<int>(x), static_cast<int>(y));

    if (pointOnSegment(intersection, p1, p2) && pointOnSegment(intersection, p3, p4)) {
        return intersection;
    }

    return std::nullopt;
}

bool Controller::pointOnSegment(const QPointF& p, const QPointF& a, const QPointF& b) const
{
    int cross = (p.x() - a.x()) * (b.y() - a.y()) - (p.y() - a.y()) * (b.x() - a.x());
    if (abs(cross) > 1e-8) return false;
    bool x_ok = (p.x() >= std::min(a.x(), b.x())) && (p.x() <= std::max(a.x(), b.x()));
    bool y_ok = (p.y() >= std::min(a.y(), b.y())) && (p.y() <= std::max(a.y(), b.y()));
    return x_ok && y_ok;
}

double Distance(const QPointF& p1, const QPointF& p2) {
    const double dx = p1.x() - p2.x();
    const double dy = p1.y() - p2.y();
    return std::sqrt((dx * dx) + (dy * dy));
}


void Controller::intersectRays(std::vector<Ray>* rays) const
{
    for (auto& ray : *rays) {
        double min_dist = -1;
        QPointF closest_intersection = ray.getEnd();

         std::vector<Polygon> all_polygons = m_polygons;
        (all_polygons).push_back(m_bounding_polygon);

        for (const auto& polygon : all_polygons) {
            auto intersection = polygon.intersectRay(ray);
            if (intersection) {
                const double dist = Distance(ray.getBegin(), *intersection);
                if (dist < min_dist || min_dist == -1) {
                    min_dist = dist;
                    closest_intersection = *intersection;
                }
            }
        }

        ray.setEnd(closest_intersection);
    }
    // for (auto& ray : *rays) {
    //     QPointF ray_begin = ray.getBegin();
    //     QPointF ray_end = ray.getEnd();
    //     QPointF closest_intersection = ray_end;
    //     double min_distance = std::numeric_limits<double>::max();

    //     const auto& all_polygons = [&]() {
    //         std::vector<Polygon> result = m_polygons;
    //         result.push_back(m_bounding_polygon);
    //         return result;
    //     }();

    //     for (const auto& polygon : all_polygons) {
    //          std::optional<QPointF> new_end = polygon.intersectRay(ray);
    //         if (new_end == std::nullopt) {
    //             continue;
    //         }
    //         double dx = new_end.value().x() - ray_begin.x();
    //         double dy = new_end.value().y() - ray_begin.y();
    //         double distance = dx * dx + dy * dy;

    //         if (distance < min_distance) {
    //             min_distance = distance;
    //             closest_intersection = new_end.value();
    //         }
    //     }

    //     ray.setEnd(closest_intersection);
    // }
}

bool Controller::pointOnRay(const QPointF& p, const QPointF& ray_begin, const QPointF& ray_end) const
{
    int cross = (p.x() - ray_begin.x()) * (ray_end.y() - ray_begin.y()) -
                (p.y() - ray_begin.y()) * (ray_end.x() - ray_begin.x());
    if (abs(cross) > 1e-8) return false;

    bool x_ok = (ray_end.x() >= ray_begin.x()) ?
                    (p.x() >= ray_begin.x()) : (p.x() <= ray_begin.x());
    bool y_ok = (ray_end.y() >= ray_begin.y()) ?
                    (p.y() >= ray_begin.y()) : (p.y() <= ray_begin.y());

    return x_ok && y_ok;
}

// void Controller::removeAdjacentRays(std::vector<Ray>* rays) const
// {
//     if (!rays || rays->size() < 2) return;

//     std::sort(rays->begin(), rays->end(), compareRaysByAngle);

//     const double epsilon = 1e-6;
//     auto new_end = std::unique(rays->begin(), rays->end(),
//                                [epsilon](const Ray& a, const Ray& b) {
//                                    QPoint a_end = a.getEnd();
//                                    QPoint b_end = b.getEnd();
//                                    double dx = a_end.x() - b_end.x();
//                                    double dy = a_end.y() - b_end.y();
//                                    return (dx * dx + dy * dy) < epsilon;
//                                });

//     rays->erase(new_end, rays->end());
// }

// double Distance(const QPointF& p1, const QPointF& p2) {
//     const double dx = p1.x() - p2.x();
//     const double dy = p1.y() - p2.y();
//     return std::sqrt((dx * dx) + (dy * dy));
// }



void Controller::removeAdjacentRays(std::vector<Ray>* rays) const {
    if (rays->empty()) {
        return;
    }
     std::sort(rays->begin(), rays->end(), compareRaysByAngle);
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



Polygon Controller::createLightArea() const
{
    auto rays = castRays();
    intersectRays(&rays);
    removeAdjacentRays(&rays);

    std::sort(rays.begin(), rays.end(), compareRaysByAngle);

    std::vector<QPointF> vertices;
    vertices.reserve(rays.size());
    for (const auto& ray : rays) {
        vertices.push_back(ray.getEnd());
    }

    return Polygon(vertices);
}

std::vector<Polygon> Controller::createLightAreas() {
    std::vector<Polygon> polys;
    for (const QPointF& l : light_sources_shades) {
        std::vector<Ray> rays = castRaysSources(l);
        intersectRays(&rays);
        removeAdjacentRays(&rays);
        std::vector<QPointF> vertices_light;
        for (const Ray& ray : rays) {
            vertices_light.push_back(ray.getEnd());
        }
        polys.push_back(Polygon(vertices_light));
    }
    return polys;
}

bool Controller::compareRaysByAngle(const Ray& a, const Ray& b)
{
    QPointF a_dir = a.getEnd() - a.getBegin();
    QPointF b_dir = b.getEnd() - b.getBegin();

    double angle_a = atan2(a_dir.y(), a_dir.x());
    double angle_b = atan2(b_dir.y(), b_dir.x());

    return angle_a < angle_b;
}


Polygon Controller::calculateLightArea() const
{
    auto rays = castRays();
    intersectRays(&rays);
    removeAdjacentRays(&rays);

    std::sort(rays.begin(), rays.end(), compareRaysByAngle);

    std::vector<QPointF> vertices;
    vertices.reserve(rays.size());
    for (const auto& ray : rays) {
        vertices.push_back(ray.getEnd());
    }

    return Polygon(vertices);
}


Polygon Controller::calculateLightAreaForSource(const QPointF& source) const
{
    auto rays = castRaysSources(source);
intersectRays(&rays);
removeAdjacentRays(&rays);

std::sort(rays.begin(), rays.end(), compareRaysByAngle);

std::vector<QPointF> vertices;
vertices.reserve(rays.size());
for (const auto& ray : rays) {
    vertices.push_back(ray.getEnd());
}

return Polygon(vertices);
}


void Controller::setLightSources(const std::vector<QPointF>& sources) {
    m_light_sources = sources;
}

std::vector<Polygon> Controller::calculateAllLightAreas() {
    std::vector<Polygon> areas;
    for (const auto& source : m_light_sources) {
        areas.push_back(calculateLightAreaForSource(source));
    }
    return areas;
}

std::vector<QPointF> Controller::getLightSources() {
    // Параметры
    const int numPoints = 5;       // Количество дополнительных точек
    const qreal radius = 10.0;     // Радиус расположения точек

    // Создаем массив точек
    std::vector<QPointF> lightPoints;

    // Добавляем главный источник в массив (если нужно)
    lightPoints.push_back(m_light_source);

    // Рассчитываем координаты дополнительных точек
    for (int i = 0; i < numPoints; ++i) {
        // Вычисляем угол для текущей точки
        qreal angle = 2 * M_PI * i / numPoints; // В радианах

        // Рассчитываем смещение относительно главного источника
        qreal xOffset = radius * std::cos(angle);
        qreal yOffset = radius * std::sin(angle);

        // Создаем точку и добавляем в массив
        QPointF point(
            m_light_source.x() + xOffset,
            m_light_source.y() + yOffset
            );
        lightPoints.push_back(point);
    }
    return m_light_sources = {lightPoints};
}
