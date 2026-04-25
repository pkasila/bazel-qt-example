#include "geometry.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>

namespace {
constexpr double kEps = 1e-9;
constexpr double kRayAngleEps = 0.0001;
constexpr double kDuplicateDistanceEps = 0.5;
constexpr double kBoundaryMargin = 2000.0;
constexpr double kPi = 3.141592653589793238462643383279502884;
constexpr std::size_t kLightSampleCount = 37;
constexpr double kLightSampleRadius = 14.0;

double Cross(const QPointF& a, const QPointF& b) {
    return a.x() * b.y() - a.y() * b.x();
}

double Dot(const QPointF& a, const QPointF& b) {
    return a.x() * b.x() + a.y() * b.y();
}

double LengthSquared(const QPointF& p) {
    return Dot(p, p);
}

double DistanceSquared(const QPointF& a, const QPointF& b) {
    return LengthSquared(a - b);
}

double NormalizeAngle(double angle) {
    const double two_pi = 2.0 * kPi;
    angle = std::fmod(angle, two_pi);
    if (angle <= -kPi) {
        angle += two_pi;
    } else if (angle > kPi) {
        angle -= two_pi;
    }
    return angle;
}

QPointF ToPointF(const QPoint& p) {
    return QPointF(p);
}

QPointF RotateAroundOrigin(const QPointF& point, double angle) {
    const double c = std::cos(angle);
    const double s = std::sin(angle);
    return QPointF(point.x() * c - point.y() * s, point.x() * s + point.y() * c);
}

std::vector<QPointF> BuildLightOffsets() {
    std::vector<QPointF> offsets;
    offsets.reserve(kLightSampleCount);
    offsets.emplace_back(0.0, 0.0);

    constexpr double kGoldenAngle = 2.39996322972865332;
    const std::size_t ring_samples = kLightSampleCount - 1;
    for (std::size_t i = 0; i < ring_samples; ++i) {
        const double t = (static_cast<double>(i) + 0.5) / static_cast<double>(ring_samples);
        const double radius = kLightSampleRadius * std::sqrt(t);
        const double angle = i * kGoldenAngle;
        offsets.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
    }

    return offsets;
}

Polygon MakeBoundaryPolygon(double width, double height) {
    std::vector<QPointF> vertices;
    vertices.emplace_back(-kBoundaryMargin, -kBoundaryMargin);
    vertices.emplace_back(width + kBoundaryMargin, -kBoundaryMargin);
    vertices.emplace_back(width + kBoundaryMargin, height + kBoundaryMargin);
    vertices.emplace_back(-kBoundaryMargin, height + kBoundaryMargin);
    return Polygon(vertices);
}

}  // namespace

Ray::Ray(const QPoint& begin, const QPoint& end, double angle)
    : begin_(ToPointF(begin)), end_(ToPointF(end)), angle_(angle) {
}

Ray::Ray(const QPointF& begin, const QPointF& end, double angle)
    : begin_(begin), end_(end), angle_(angle) {
}

const QPointF& Ray::GetBegin() const {
    return begin_;
}

const QPointF& Ray::GetEnd() const {
    return end_;
}

double Ray::GetAngle() const {
    return angle_;
}

void Ray::SetBegin(const QPoint& begin) {
    begin_ = ToPointF(begin);
}

void Ray::SetBegin(const QPointF& begin) {
    begin_ = begin;
}

void Ray::SetEnd(const QPoint& end) {
    end_ = ToPointF(end);
}

void Ray::SetEnd(const QPointF& end) {
    end_ = end;
}

void Ray::SetAngle(double angle) {
    angle_ = angle;
}

Ray Ray::Rotate(double angle) const {
    const QPointF delta = end_ - begin_;
    const QPointF rotated = RotateAroundOrigin(delta, angle);
    return Ray(begin_, begin_ + rotated, NormalizeAngle(angle_ + angle));
}

Polygon::Polygon() = default;

Polygon::Polygon(const std::vector<QPoint>& vertices) {
    vertices_.reserve(vertices.size());
    for (const auto& vertex : vertices) {
        vertices_.push_back(ToPointF(vertex));
    }
}

Polygon::Polygon(const std::vector<QPointF>& vertices)
    : vertices_(vertices) {
}

const std::vector<QPointF>& Polygon::GetVertices() const {
    return vertices_;
}

void Polygon::AddVertex(const QPoint& vertex) {
    vertices_.push_back(ToPointF(vertex));
}

void Polygon::AddVertex(const QPointF& vertex) {
    vertices_.push_back(vertex);
}

void Polygon::UpdateLastVertex(const QPoint& new_vertex) {
    if (!vertices_.empty()) {
        vertices_.back() = ToPointF(new_vertex);
    }
}

void Polygon::UpdateLastVertex(const QPointF& new_vertex) {
    if (!vertices_.empty()) {
        vertices_.back() = new_vertex;
    }
}

std::optional<QPointF> Polygon::IntersectRay(const Ray& ray) const {
    if (vertices_.size() < 2) {
        return std::nullopt;
    }

    const QPointF begin = ray.GetBegin();
    const QPointF direction = ray.GetEnd() - begin;
    if (LengthSquared(direction) < kEps) {
        return std::nullopt;
    }

    double best_t = std::numeric_limits<double>::infinity();
    QPointF best_point;

    const std::size_t vertex_count = vertices_.size();
    for (std::size_t i = 0; i < vertex_count; ++i) {
        const QPointF a = vertices_[i];
        const QPointF b = vertices_[(i + 1) % vertex_count];
        const QPointF edge = b - a;
        const double denom = Cross(direction, edge);
        if (std::abs(denom) < kEps) {
            continue;
        }

        const QPointF ap = a - begin;
        const double t = Cross(ap, edge) / denom;
        const double u = Cross(ap, direction) / denom;

        if (t < -kEps || t > 1.0 + kEps) {
            continue;
        }
        if (u < -kEps || u > 1.0 + kEps) {
            continue;
        }

        if (t < best_t) {
            best_t = t;
            best_point = begin + direction * t;
        }
    }

    if (!std::isfinite(best_t)) {
        return std::nullopt;
    }

    return best_point;
}

std::size_t Polygon::Size() const {
    return vertices_.size();
}

bool Polygon::Empty() const {
    return vertices_.empty();
}

Controller::Controller()
    : light_offsets_(BuildLightOffsets()) {
    SetBounds(800.0, 600.0);
    SetLightSource(QPointF(240.0, 180.0));
}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return polygons_;
}

void Controller::AddPolygon(const Polygon& polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddPolygon(const std::vector<QPoint>& vertices) {
    polygons_.emplace_back(vertices);
}

void Controller::AddPolygon(const std::vector<QPointF>& vertices) {
    polygons_.emplace_back(vertices);
}

void Controller::AddVertexToLastPolygon(const QPoint& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().AddVertex(new_vertex);
    }
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().AddVertex(new_vertex);
    }
}

void Controller::UpdateLastPolygon(const QPoint& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().UpdateLastVertex(new_vertex);
    }
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!polygons_.empty()) {
        polygons_.back().UpdateLastVertex(new_vertex);
    }
}

void Controller::FinalizeLastPolygon() {
    if (polygons_.size() <= 1) {
        return;
    }

    const auto vertices = polygons_.back().GetVertices();
    if (vertices.empty()) {
        polygons_.pop_back();
        return;
    }

    std::vector<QPointF> committed(vertices.begin(), vertices.end() - 1);
    if (committed.size() < 3) {
        polygons_.pop_back();
        return;
    }

    polygons_.back() = Polygon(committed);
}

void Controller::SetBounds(double width, double height) {
    if (polygons_.empty()) {
        polygons_.push_back(MakeBoundaryPolygon(width, height));
        return;
    }

    polygons_.front() = MakeBoundaryPolygon(width, height);
}

QPointF Controller::GetLightSource() const {
    if (light_sources_.empty()) {
        return QPointF();
    }
    return light_sources_.front();
}

void Controller::SetLightSource(const QPoint& light_source) {
    SetLightSource(ToPointF(light_source));
}

void Controller::SetLightSource(const QPointF& light_source) {
    if (light_offsets_.empty()) {
        light_offsets_ = BuildLightOffsets();
    }

    light_sources_.resize(light_offsets_.size());
    light_sources_.front() = light_source;
    for (std::size_t i = 1; i < light_offsets_.size(); ++i) {
        light_sources_[i] = light_source + light_offsets_[i];
    }
}

const std::vector<QPointF>& Controller::GetLightSources() const {
    return light_sources_;
}

void Controller::SetLightSources(const std::vector<QPointF>& lights) {
    light_sources_ = lights;
}

std::vector<Ray> Controller::CastRays() const {
    std::vector<Ray> rays;
    for (const auto& source : light_sources_) {
        auto source_rays = CastRays(source);
        rays.insert(rays.end(), source_rays.begin(), source_rays.end());
    }
    return rays;
}

std::vector<Ray> Controller::CastRays(const QPointF& light_source) const {
    std::vector<Ray> rays;
    for (const auto& polygon : polygons_) {
        for (const auto& vertex : polygon.GetVertices()) {
            const QPointF delta = vertex - light_source;
            if (LengthSquared(delta) < kEps) {
                continue;
            }

            const double angle = NormalizeAngle(std::atan2(delta.y(), delta.x()));
            const Ray base(light_source, vertex, angle);
            rays.push_back(base.Rotate(-kRayAngleEps));
            rays.push_back(base);
            rays.push_back(base.Rotate(kRayAngleEps));
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays) const {
    if (rays == nullptr) {
        return;
    }

    for (Ray& ray : *rays) {
        const QPointF begin = ray.GetBegin();
        const QPointF original_end = ray.GetEnd();
        const double original_distance_sq = DistanceSquared(begin, original_end);
        QPointF closest = original_end;
        double closest_distance_sq = original_distance_sq;

        for (const auto& polygon : polygons_) {
            const auto intersection = polygon.IntersectRay(ray);
            if (!intersection.has_value()) {
                continue;
            }

            const double intersection_distance_sq = DistanceSquared(begin, *intersection);
            if (intersection_distance_sq + kEps < closest_distance_sq) {
                closest_distance_sq = intersection_distance_sq;
                closest = *intersection;
            }
        }

        ray.SetEnd(closest);
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays) const {
    if (rays == nullptr || rays->empty()) {
        return;
    }

    std::sort(rays->begin(), rays->end(), [](const Ray& lhs, const Ray& rhs) {
        if (std::abs(lhs.GetAngle() - rhs.GetAngle()) > 1e-12) {
            return lhs.GetAngle() < rhs.GetAngle();
        }
        const double lhs_distance = DistanceSquared(lhs.GetBegin(), lhs.GetEnd());
        const double rhs_distance = DistanceSquared(rhs.GetBegin(), rhs.GetEnd());
        return lhs_distance < rhs_distance;
    });

    std::vector<Ray> filtered;
    filtered.reserve(rays->size());

    for (const auto& ray : *rays) {
        if (filtered.empty()) {
            filtered.push_back(ray);
            continue;
        }

        if (DistanceSquared(filtered.back().GetEnd(), ray.GetEnd()) > kDuplicateDistanceEps * kDuplicateDistanceEps) {
            filtered.push_back(ray);
        }
    }

    if (filtered.size() > 1 &&
        DistanceSquared(filtered.front().GetEnd(), filtered.back().GetEnd()) <= kDuplicateDistanceEps * kDuplicateDistanceEps) {
        filtered.pop_back();
    }

    *rays = std::move(filtered);
}

QPointF Controller::IntersectWithBoundary(const QPointF& source, double angle) const {
    if (polygons_.empty()) {
        return source;
    }

    const double ray_length = 2000.0;
    const QPointF far_point(
        source.x() + ray_length * std::cos(angle),
        source.y() + ray_length * std::sin(angle)
    );

    Ray ray(source, far_point, angle);
    const auto intersection = polygons_.front().IntersectRay(ray);
    if (intersection.has_value()) {
        return *intersection;
    }

    return far_point;
}

Polygon Controller::CreateLightArea() const {
    return CreateLightArea(GetLightSource());
}

Polygon Controller::CreateLightArea(const QPoint& light_source) const {
    return CreateLightArea(QPointF(light_source));
}

Polygon Controller::CreateLightArea(const QPointF& light_source) const {
    const QPoint source = light_source.toPoint();
    std::vector<Ray> rays;

    for (int i = 0; i < 360; i += 1) {
        const double angle = i * kPi / 180.0;
        const QPoint far_point(
            source.x() + static_cast<int>(2000 * std::cos(angle)),
            source.y() + static_cast<int>(2000 * std::sin(angle))
        );

        Ray ray(source, far_point, angle);
        QPointF closest_point = far_point;
        double min_distance = 2000.0;
        bool has_intersection = false;
        for (std::size_t index = 1; index < polygons_.size(); ++index) {
            const auto& polygon = polygons_[index];
            const auto intersection = polygon.IntersectRay(ray);
            if (intersection.has_value()) {
                has_intersection = true;
                const double distance = std::hypot(
                    intersection->x() - source.x(),
                    intersection->y() - source.y()
                );
                if (distance < min_distance && distance > 1e-6) {
                    min_distance = distance;
                    closest_point = *intersection;
                }
            }
        }
        if (!has_intersection) {
            closest_point = IntersectWithBoundary(QPointF(source), angle);
        }

        ray.SetEnd(closest_point);
        rays.push_back(ray);
    }

    std::sort(rays.begin(), rays.end(),
        [](const Ray& a, const Ray& b) {
            return a.GetAngle() < b.GetAngle();
        });

    std::vector<QPointF> vertices;
    for (const auto& ray : rays) {
        vertices.push_back(ray.GetEnd());
    }

    vertices.erase(std::unique(vertices.begin(), vertices.end(),
        [](const QPointF& a, const QPointF& b) {
            return std::abs(a.x() - b.x()) < 2 && std::abs(a.y() - b.y()) < 2;
        }), vertices.end());

    if (!vertices.empty()) {
        vertices.push_back(vertices.front());
    }

    return Polygon(vertices);
}

std::vector<Polygon> Controller::CreateLightAreas() const {
    std::vector<Polygon> areas;
    areas.reserve(light_sources_.size());
    for (const auto& source : light_sources_) {
        areas.push_back(CreateLightArea(source));
    }
    return areas;
}
