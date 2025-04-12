#include <QPoint>
#include <QPointF>

#include <limits>
#include <optional>
#include <ranges>
#include <span>
#include <vector>

#include "math.h" // NOLINT

const auto DOUBLE_MAX = std::numeric_limits<double>::max();

std::optional<QPointF> intersect_segment_segment(const Segment left, const Segment right)
{
    const auto epsilon = 0.0000001;
    auto [rp_x, rp_y] = QPointF(left.from);
    auto [rd_x, rd_y] = left.to - left.from;

    auto [vp_x, vp_y] = QPointF(right.from);
    auto [vd_x, vd_y] = right.to - right.from;

    auto rp = vd_x * rd_y - vd_y * rd_x;
    if (rp == 0.0) {
        // Parallel
        return std::nullopt;
    }
    auto t2 = (rd_x * (vp_y - rp_y) - rd_y * (vp_x - rp_x)) / rp;
    if (t2 < -epsilon || t2 > 1.0 + epsilon) {
        // Outside of segment
        return std::nullopt;
    }

    auto t1 = std::numeric_limits<double>::quiet_NaN();
    // Avoid case when rd_{x,y} ~ 0.0
    if (std::fabs(rd_x) > std::fabs(rd_y)) {
        t1 = (vp_x + vd_x * t2 - rp_x) / rd_x;
    } else {
        t1 = (vp_y + vd_y * t2 - rp_y) / rd_y;
    }
    if (t1 < -epsilon || t1 > 1.0 + epsilon) {
        // Opposite of the ray
        return std::nullopt;
    }
    return QPointF(rp_x, rp_y) + QPointF(rd_x, rd_y) * t1;
}

std::optional<Intersection> intersect_ray_segment(const Ray ray, const Segment segment)
{
    const auto epsilon = 0.0000001;
    auto [rp_x, rp_y] = ray.position;
    auto [rd_x, rd_y] = ray.direction.into();
    auto [vp_x, vp_y] = QPointF(segment.from);
    auto [vd_x, vd_y] = segment.to - segment.from;

    auto rp = vd_x * rd_y - vd_y * rd_x;
    if (rp == 0.0) {
        // Parallel
        return std::nullopt;
    }
    auto t2 = (rd_x * (vp_y - rp_y) - rd_y * (vp_x - rp_x)) / rp;
    if (t2 < -epsilon || t2 > 1.0 + epsilon) {
        // Outside of segment
        return std::nullopt;
    }

    auto t1 = std::numeric_limits<double>::quiet_NaN();
    // Avoid case when rd_{x,y} ~ 0.0
    if (std::fabs(rd_x) > std::fabs(rd_y)) {
        t1 = (vp_x + vd_x * t2 - rp_x) / rd_x;
    } else {
        t1 = (vp_y + vd_y * t2 - rp_y) / rd_y;
    }
    if (t1 < -epsilon) {
        // Opposite of the ray
        return std::nullopt;
    }

    return Intersection(t1);
}

std::optional<Intersection> intersect_ray_poly(const Ray ray, const std::span<Polygon> polygons,
                                               const double distance_cutoff)
{
    const auto epsilon = 0.001;
    auto min_intersection = Intersection(DOUBLE_MAX);

    for (const auto &poly : polygons) {
        assert(poly.verteces.size() != 0);
        if (poly.verteces.size() == 1) {
            continue;
        }
        auto to_check = poly.verteces.size();
        if (!poly.enclosed) {
            to_check -= 1;
        }
        for (auto i : std::views::iota(0UL, to_check)) {
            auto from = poly.verteces[i];
            auto to = poly.verteces[(i + 1) % poly.verteces.size()];
            auto too_close = [](auto left, auto right) {
                auto diff = left - right;
                return QPointF::dotProduct(diff, diff) < 0.0001;
            };
            if (too_close(ray.position, from) || too_close(ray.position, to)) {
                continue;
            }

            auto intersection = intersect_ray_segment(ray, Segment(from, to));
            if (intersection.has_value()) {
                if (intersection.value().distance < min_intersection.distance + 0.001) {
                    min_intersection.distance = intersection.value().distance;
                    if (min_intersection.distance < distance_cutoff - epsilon) {
                        return min_intersection;
                    }
                }
            }
        }
    }
    if (min_intersection.distance == DOUBLE_MAX) {
        return std::nullopt;
    }
    return min_intersection;
}

void Intersections::find_intersections(const QPointF light, const std::span<Polygon> poly,
                                       const std::span<QPointF> verteces)
{
    this->buf.clear();

    auto angle = 0.0001;
    auto ccw = Direction::X().rotate_angle(angle);
    auto cw = Direction::X().rotate_angle(-angle);

    auto too_close = [](auto left, auto right) {
        auto diff = left - right;
        // Less than a pixel difference won't be noticeable anyway
        return QPointF::dotProduct(diff, diff) <= 1.0;
    };

    for (const auto &vertex : verteces) {
        if (too_close(vertex, light)) {
            continue;
        }
        auto distance = std::sqrt(QPointF::dotProduct(vertex - light, vertex - light));
        auto ray = Ray::from_points(light, vertex);
        auto intersection = intersect_ray_poly(ray, poly, distance);
        if (intersection.has_value()) {
            auto min_intersection = intersection.value();
            if (distance <= min_intersection.distance + 4.0) {
                auto ray_ccw = ray.rotate_vector(ccw);
                auto point_ccw = intersect_ray_poly(ray_ccw, poly, 0.0);
                if (point_ccw.has_value()) {
                    auto point = point_ccw.value().point(ray_ccw);
                    if (!too_close(point, vertex)) {
                        this->buf.emplace_back(point);
                    }
                }

                this->buf.emplace_back(vertex);

                auto ray_cw = ray.rotate_vector(cw);
                auto point_cw = intersect_ray_poly(ray_cw, poly, 0.0);
                if (point_cw.has_value()) {
                    auto point = point_cw.value().point(ray_cw);
                    if (!too_close(point, vertex)) {
                        this->buf.emplace_back(point);
                    }
                }
            }
        }
    }
}

QPointF clamp_point(QPointF point, QRectF rect, double offset)
{
    auto top_left = rect.topLeft() + QPointF(offset, offset);
    auto bottom_right = rect.bottomRight() - QPointF(offset, offset);
    return { std::clamp(point.x(), top_left.x(), bottom_right.x()),
             std::clamp(point.y(), top_left.y(), bottom_right.y()) };
}

bool point_inside(QPointF point, QRectF rect, double offset)
{
    return rect.topLeft().x() - offset <= point.x() && point.x() <= rect.bottomRight().x() + offset
            && rect.topLeft().y() - offset <= point.y()
            && point.y() <= rect.bottomRight().y() + offset;
}

void Intersections::compute_visible(Polygon &output, QPointF light, std::span<Polygon> poly,
                                    std::span<QPointF> susp)
{
    this->find_intersections(light, poly, susp);

    std::sort(buf.begin(), buf.end(), [light](const auto &left, const auto &right) {
        auto l = left - light;
        auto r = right - light;
        // Consider
        //  `auto lx = l.x() / std::sqrt(QPointF::dotProduct(l, l));
        //  `auto rx = r.x() / std::sqrt(QPointF::dotProduct(r, r));
        // Then `lx < rx` is the same as
        auto lx = l.x() * std::abs(l.x()) * QPointF::dotProduct(r, r);
        auto rx = r.x() * std::abs(r.x()) * QPointF::dotProduct(l, l);
        auto is_neg = [](double x) { return x < 0.0; };
        if (is_neg(l.y()) != is_neg(r.y())) {
            return is_neg(r.y());
        }
        if (is_neg(l.y())) {
            return lx > rx;
        }
        return lx < rx;
    });

    output.enclosed = true;
    output.verteces.clear();
    output.verteces.reserve(buf.size());
    for (const auto &p : buf) {
        output.verteces.push_back(p);
    }
}

void Intersections::compute_visible_polygons(Light &light, std::span<Polygon> poly,
                                             std::span<QPointF> susp)
{
    auto light_shards = this->light_shards - 1;

    auto dir = Direction::X();
    auto vec = Direction::X().rotate_angle(2.0 * M_PI / light_shards);

    if (light.visible.size() == 0) {
        light.visible.emplace_back();
        compute_visible(light.visible[0], light.position, poly, susp);
    }
    for (auto i : std::views::iota(1, light_shards)) {
        if (light.visible.size() == i) {
            light.visible.emplace_back();
            compute_visible(light.visible[i], light.position + dir * light_radius, poly, susp);
        }
        dir = dir.rotate_vector(vec);
    }
};
