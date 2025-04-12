#include <QPoint>
#include <QPointF>
#include <QRect>

#include <optional>
#include <span>
#include <vector>

struct Direction : public QPointF
{
    static Direction X() { return { QPointF(1.0, 0.0) }; }
    Direction(QPointF dir) : QPointF(dir) { } // NOLINT(*-explicit-*)
    [[nodiscard]] QPointF into() const { return *this; }
    [[nodiscard]] Direction rotate_vector(Direction vector) const
    {
        auto [a, b] = this->into();
        auto [c, d] = vector.into();
        return { QPointF(a * c - b * d, a * d + b * c) };
    }
    [[nodiscard]] Direction rotate_angle(double angle) const
    {
        return this->rotate_vector(QPointF(std::cos(angle), std::sin(angle)));
    }
    [[nodiscard]] Direction normalized() const
    {
        return { *this / std::sqrt(QPointF::dotProduct(*this, *this)) };
    }
};

struct Ray
{
    // All my homies hate set/getters.
    QPointF position;
    Direction direction;

    [[nodiscard]] static Ray from_points(QPointF from, QPointF to)
    {
        if (from == to) {
            to = from + QPointF(1.0, 0.0);
        }
        return Ray(from, Direction(to - from).normalized());
    }
    [[nodiscard]] Ray rotate_vector(QPointF vector) const
    {
        return Ray(position, direction.rotate_vector(vector)); //
    }
    [[nodiscard]] Ray rotate(double angle) const
    {
        return Ray(position, direction.rotate_angle(angle)); //
    }
};

struct Polygon
{
    std::vector<QPointF> verteces;
    bool enclosed = false;
};

struct Segment
{
    QPointF from;
    QPointF to;
};

struct Intersection
{
    double distance;

    [[nodiscard]] QPointF point(const Ray &ray) const
    {
        return ray.position + ray.direction * distance;
    }
};

struct Light
{
    QPointF position;
    std::vector<Polygon> visible;

    void invalidate() { visible.clear(); }
    [[nodiscard]] bool is_cached() const { return !visible.empty(); }
};

struct Intersections
{
    std::vector<QPointF> buf;
    int light_shards;
    float light_radius;

    void find_intersections(QPointF light, std::span<Polygon> poly, std::span<QPointF> verteces);
    void compute_visible(Polygon &output, QPointF light, std::span<Polygon> poly,
                         std::span<QPointF> susp);
    void compute_visible_polygons(Light &light, std::span<Polygon> poly, std::span<QPointF> susp);
};

std::optional<QPointF> intersect_segment_segment(Segment left, Segment right);
std::optional<Intersection> intersect_ray_segment(Ray ray, Segment segment);

QPointF clamp_point(QPointF point, QRectF rect, double offset);
bool point_inside(QPointF point, QRectF rect, double offset);
