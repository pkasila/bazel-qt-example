#include "Controller.h"
#include <iostream>
#include <QtWidgets>
#include <QApplication>


const double compare_value = 0.0001;


Controller::Controller() {

    setMouseTracking(true);

    QWidget *central_widget = new QWidget();
    central_widget = new QWidget();
    setCentralWidget(central_widget);

    mode_box_ = new QComboBox(central_widget);
    mode_box_->addItems({"light", "polygons"});

    paint_widget_ = new PaintWidget(central_widget);


    QVBoxLayout *mode_layout = new QVBoxLayout();
    QVBoxLayout *painter_layout = new QVBoxLayout();

    mode_layout->addWidget(mode_box_);
    painter_layout->addWidget(paint_widget_);

    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->addLayout(mode_layout);
    main_layout->addLayout(painter_layout);

    central_widget->setLayout(main_layout);
    CreateActions();

    std::vector<QPointF> vertices;
    Polygon polygon({
        QPointF(0, 0), QPointF(qApp->screens()[0]->size().width(), 0),
        QPointF(qApp->screens()[0]->size().width(), qApp->screens()[0]->size().height()),
        QPointF(0, qApp->screens()[0]->size().height()),
        QPointF(0, 0)
    });
    AddPolygon(polygon);
}

QPointF Controller::GetLightSource() {
    return light_source_;
}

void Controller::SetLightSource(QPointF lightSource) {
    light_source_ = lightSource;
}

const std::vector<Polygon> &Controller::GetPolygons() {
    return polygons_;
}

void Controller::AddPolygon(const Polygon &polygon) {
    polygons_.push_back(polygon);
}

void Controller::AddVertexToLastPolygon(const QPointF &new_vertex) {
    Polygon &last_polygon = polygons_[polygons_.size() - 1];
    last_polygon.AddVertex(new_vertex);
}

// TODO
void Controller::UpdateLastPolygon(const QPointF &new_vertex) {
    // auto& last_polygon = current_polygon_.;
    // last_polygon = new_vertex;
}

std::vector<Ray> Controller::CastRays(QPointF light_source) {
    std::vector<Ray> rays;

    for (auto& polygon : polygons_) {
        std::vector<QPointF> vertices = polygon.GetVertices();

        for (const QPointF& vertex : vertices) {
            // Calculate the angle to the vertex
            double angle = AngleBetweenPoints(vertex, light_source);

            // Create the primary ray pointing to the vertex
            rays.push_back(Ray(light_source, vertex, angle));

            // Add the two neighbour rays
            rays.push_back(FindNeighbourRay(light_source, angle + compare_value));
            rays.push_back(FindNeighbourRay(light_source, angle - compare_value));
        }
    }

    return rays;
}

void Controller::IntersectRays(std::vector<Ray> *rays, QPointF light_source) {
    for (auto polygon: polygons_) {
        for (auto &ray: *rays) {
            auto intersection_result = polygon.IntersectRay(ray);
            if (intersection_result != std::nullopt) {
                if (Polygon::GetDistance(ray.GetBegin(), intersection_result.value()) <
                    Polygon::GetDistance(ray.GetBegin(), ray.GetEnd())) {
                    double angle1 = AngleBetweenPoints(intersection_result.value(), light_source);
                    double angle2 = AngleBetweenPoints(ray.GetEnd(), light_source);

                    if (CompareAngles(angle1, angle2)) {
                        ray.SetEnd(intersection_result.value());
                    }
                }
            }
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray> *rays) {
}

Polygon Controller::CreateLightArea(QPointF light_source) {
    std::vector<Ray> rays = CastRays(light_source);
    IntersectRays(&rays, light_source);
    RemoveAdjacentRays(&rays);

    std::sort(rays.begin(), rays.end(), [](const Ray &ray1, const Ray &ray2) {
        return ray1.GetAngle() < ray2.GetAngle();
    });

    std::vector<QPointF> vertices;
    for (auto ray: rays) {
        vertices.push_back(ray.GetEnd());
    }
    vertices.push_back(vertices[0]);
    return Polygon(vertices);
}


void Controller::ModeChanged() {

    if (mode_box_->currentIndex() == LIGHT) {
        if (!is_right_clicked) {
            AddVertexToLastPolygon(polygons_[polygons_.size() - 1].GetVertices()[0]);
            is_right_clicked = true;
        }
        // create a small red circle cursor
        QPixmap pixmap(30, 30);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
        painter.drawEllipse(15, 15, 5, 5);

        double radius = 10.0;
        for (int i = 0; i < 10; ++i) {
            // get position of secondary light source
            double angle = 2 * M_PI * i / 10;
            painter.drawEllipse(15 + radius * cos(angle), 15 + radius * sin(angle), 5, 5);
        }

        // Set the cursor with hotspot at center
        QCursor customCursor(pixmap, 15, 15);
        setCursor(customCursor);

        mode_ = LIGHT;
    } else {
        paint_widget_->ClearRaysBuffer();
        paint_widget_->update();
        // Restore default cursor
        setCursor(Qt::ArrowCursor);
        mode_ = POLYGONS;
    }
}

void Controller::MouseMoveEvent(QPointF pos) {
    if (mode_ == LIGHT) {
        light_source_ = pos;
        paint_widget_->ClearRaysBuffer();

        // Main light
        std::vector<Ray> rays = CastRays(light_source_);
        Polygon main_polygon = CreateLightArea(light_source_);
        paint_widget_->PaintRays(main_polygon, QColor(255, 255, 255, 255));
        // IntersectRays(&rays, light_source_);
        // paint_widget_->DrawRays(light_source_, rays); // Draw the rays

        // Secondary lights
        const int additional_lights = 10;
        const double radius = 7.0;
        double color_id = 235;

        for (int i = 0; i < additional_lights; ++i) {
            double angle = 2 * M_PI * i / additional_lights;
            QPointF secondary_light = light_source_ + QPointF(radius * cos(angle), radius * sin(angle));

            std::vector<Ray> sec_rays = CastRays(secondary_light);
            Polygon sec_polygon = CreateLightArea(secondary_light);
            paint_widget_->PaintRays(sec_polygon, QColor(color_id, color_id, color_id, 255));
            // IntersectRays(&sec_rays, secondary_light);
            // paint_widget_->DrawRays(secondary_light, sec_rays);

            color_id -= 20;
        }
    }
}

void Controller::MouseLeftClicked(QPointF pos) {
    if (mode_ == POLYGONS) {
        if (is_right_clicked) {
            Polygon polygon;
            AddPolygon(polygon);
            is_right_clicked = false;
        }
        AddVertexToLastPolygon(pos);
        paint_widget_->PaintPolygon(polygons_[polygons_.size() - 1]);
    }
}

void Controller::MouseRightClicked(QPointF pos) {
    if (mode_ == POLYGONS) {
        AddVertexToLastPolygon(polygons_[polygons_.size() - 1].GetVertices()[0]);
        is_right_clicked = true;
    }
}

double Controller::AngleBetweenPoints(const QPointF &p1, const QPointF &p2) {
    double dx = p2.x() - p1.x();
    double dy = p2.y() - p1.y();

    return qAtan2(dy, dx);
}

bool Controller::CompareAngles(double angle1, double angle2) {
    double diff = qAbs(angle1 - angle2);
    diff = qMin(diff, 6.28 - diff);
    return diff <= compare_value * 0.1;
}

Ray Controller::FindNeighbourRay(QPointF light_source, double angle_radians) {

    // Define a large distance to extend the ray
    const double ray_length = qApp->screens()[0]->size().width() * 2;

    // Calculate the endpoint of the main ray
    QPointF endpoint = QPointF(
        light_source.x() - ray_length * qCos(angle_radians),
        light_source.y() - ray_length * qSin(angle_radians)
    );

    // Return the ray from the light source to the calculated endpoint
    return Ray(light_source, endpoint, angle_radians);
}

Controller::~Controller() {
}

void Controller::CreateActions() {
    connect(mode_box_, &QComboBox::currentIndexChanged, this, &Controller::ModeChanged);
    connect(paint_widget_, &PaintWidget::mouseMoved, this, &Controller::MouseMoveEvent);
    connect(paint_widget_, &PaintWidget::leftClicked, this, &Controller::MouseLeftClicked);
    connect(paint_widget_, &PaintWidget::rightClicked, this, &Controller::MouseRightClicked);
}