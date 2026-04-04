#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QPointF>
#include <QKeyEvent>
#include <vector>
#include <optional>
#include <cmath>
#include <algorithm>

struct MathUtils {
    static double distance(const QPointF& a, const QPointF& b) {
        return std::sqrt(std::pow(a.x() - b.x(), 2) + std::pow(a.y() - b.y(), 2));
    }

    static std::optional<QPointF> intersect(const QPointF& p, const QPointF& p2, 
                                           const QPointF& q, const QPointF& q2) {
        double r_x = p2.x() - p.x();
        double r_y = p2.y() - p.y();
        double s_x = q2.x() - q.x();
        double s_y = q2.y() - q.y();

        double denominator = r_x * s_y - r_y * s_x;
        if (std::abs(denominator) < 1e-9) return std::nullopt;

        double t = ((q.x() - p.x()) * s_y - (q.y() - p.y()) * s_x) / denominator;
        double u = ((q.x() - p.x()) * r_y - (q.y() - p.y()) * r_x) / denominator;

        if (t >= 0 && u >= 0 && u <= 1) {
            return QPointF(p.x() + t * r_x, p.y() + t * r_y);
        }
        return std::nullopt;
    }
};

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end) : begin_(begin), end_(end) {
        angle_ = std::atan2(end.y() - begin.y(), end.x() - begin.x());
    }

    QPointF getBegin() const { return begin_; }
    QPointF getEnd() const { return end_; }
    void setEnd(const QPointF& end) { end_ = end; }
    double getAngle() const { return angle_; }

    Ray Rotate(double delta_angle) const {
        double new_angle = angle_ + delta_angle;
        QPointF new_end(begin_.x() + std::cos(new_angle) * 3000, 
                        begin_.y() + std::sin(new_angle) * 3000);
        return Ray(begin_, new_end);
    }

private:
    QPointF begin_, end_;
    double angle_;
};

class Polygon {
public:
    Polygon() = default;
    Polygon(const std::vector<QPointF>& vertices) : vertices_(vertices) {}

    const std::vector<QPointF>& getVertices() const { return vertices_; }
    void AddVertex(const QPointF& v) { vertices_.push_back(v); }

    std::optional<QPointF> IntersectRay(const Ray& ray) const {
        std::optional<QPointF> closest;
        double min_dist = 1e18;

        for (size_t i = 0; i < vertices_.size(); ++i) {
            QPointF v1 = vertices_[i];
            QPointF v2 = vertices_[(i + 1) % vertices_.size()];
            
            auto hit = MathUtils::intersect(ray.getBegin(), ray.getEnd(), v1, v2);
            if (hit) {
                double d = MathUtils::distance(ray.getBegin(), *hit);
                if (d < min_dist) {
                    min_dist = d;
                    closest = hit;
                }
            }
        }
        return closest;
    }

private:
    std::vector<QPointF> vertices_;
};

class Controller {
public:
    void AddPolygon(const Polygon& p) { polygons_.push_back(p); }
    const std::vector<Polygon>& GetPolygons() const { return polygons_; }
    void AddVertexToLastPolygon(const QPointF& v) { if (!polygons_.empty()) polygons_.back().AddVertex(v); }
    
    void setLightSource(const QPointF& p) { light_source_ = p; }
    QPointF getLightSource() const { return light_source_; }

    Polygon CreateLightArea(QPointF source) {
        std::vector<Ray> rays;
        for (const auto& poly : polygons_) {
            for (const auto& v : poly.getVertices()) {
                Ray base(source, v);
                rays.push_back(base);
                rays.push_back(base.Rotate(0.0001));
                rays.push_back(base.Rotate(-0.0001));
            }
        }

        for (auto& ray : rays) {
            for (const auto& poly : polygons_) {
                auto hit = poly.IntersectRay(ray);
                if (hit) {
                    if (MathUtils::distance(source, *hit) < MathUtils::distance(source, ray.getEnd())) {
                        ray.setEnd(*hit);
                    }
                }
            }
        }

        std::sort(rays.begin(), rays.end(), [](const Ray& a, const Ray& b) {
            return a.getAngle() < b.getAngle();
        });

        std::vector<QPointF> light_verts;
        for (const auto& r : rays) light_verts.push_back(r.getEnd());
        return Polygon(light_verts);
    }

private:
    std::vector<Polygon> polygons_;
    QPointF light_source_;
};

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    enum Mode { LIGHT, POLYGONS } mode = POLYGONS;
    bool drawing = false;
    Controller controller;
    QRadioButton *lightBtn, *polyBtn;
};

#endif