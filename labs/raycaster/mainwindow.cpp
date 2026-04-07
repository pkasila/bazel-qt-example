#include "mainwindow.h"

#include <QBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QGroupBox>
#include <QButtonGroup>
#include <algorithm>


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget* controlPanel = new QWidget();
    controlPanel->setFixedHeight(65);
    CreateModeSelector(controlPanel);

    drawing_area_ = new QWidget();
    drawing_area_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    drawing_area_->setFocusPolicy(Qt::StrongFocus);

    mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(drawing_area_, 1);

    UpdateBorderPolygon();

    drawing_area_->setFocus();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == drawing_area_ && event->type() == QEvent::MouseButtonPress) {
        drawing_area_->setFocus();
        auto* const mouse_event = dynamic_cast<QMouseEvent*>(event);
        mousePressEvent(mouse_event);
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::OnModeChanged(const int mode) {
    if (mode == 0) {
        mode_ = "light";
        if (creating_polygon_) {
            if (controller_.GetPolygons().back().GetVertices().size() < 2) {
                controller_.GetPolygons().pop_back();
            }
            creating_polygon_ = false;
        }
    } else if (mode == 1) {
        mode_ = "polygons";
    } else if (mode == 2) {
        mode_ = "static-lights";
        if (creating_polygon_) {
            if (controller_.GetPolygons().back().GetVertices().size() < 2) {
                controller_.GetPolygons().pop_back();
            }
            creating_polygon_ = false;
        }
    }
    update();
}

void MainWindow::CreateModeSelector(QWidget* parent) {
    QGroupBox* mode_box = new QGroupBox("", parent);

    mode_combo = new QComboBox(mode_box);
    mode_combo->addItem("Light mode");
    mode_combo->addItem("Polygon mode");
    mode_combo->addItem("Static mode");

    mode_combo->setMinimumWidth(180);
    mode_combo->setCurrentIndex(0);
    mode_ = "light";

    connect(mode_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::OnModeChanged);

    QVBoxLayout* radio_layout = new QVBoxLayout();
    radio_layout->addWidget(mode_combo);

    QVBoxLayout* mainLayout = new QVBoxLayout(parent);
    mainLayout->addWidget(mode_box);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mode_box->setLayout(radio_layout);
}


void MainWindow::UpdateBorderPolygon() {
    const std::vector border = {
                                QPointF(1, 1), QPointF(this->width() - 1, 1),
                                QPointF(this->width() - 1, this->height() - 65 - 1), QPointF(1, this->height() - 65 - 1)};

    if (controller_.GetPolygons().empty()) {
        controller_.AddPolygon(Polygon(border));
    } else {
        controller_.GetPolygons()[0] = Polygon(border);
    }
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    const std::vector border = {
                                QPointF(1, 1), QPointF(this->width() - 1, 1),
                                QPointF(this->width() - 1, this->height() - 65 - 1), QPointF(1, this->height() - 65 - 1)};

    for (auto& polygon : controller_.GetPolygons()) {
        for (int i = polygon.GetVertices().size() - 1; i >= 0 ; --i) {
            if (!IsPointInPolygon(polygon.GetVertices()[i], border)) {
                polygon.DeleteVertex(i);
            }
        }
    }
    UpdateBorderPolygon();
    update();
}

void MainWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const QRect drawing_rect = drawing_area_->geometry();
    painter.fillRect(drawing_rect, Qt::black);
    painter.save();
    painter.translate(drawing_rect.topLeft());

    DrawPolygons(painter);

    if (mode_ == "light" || mode_ == "static-lights") {
        DrawLightArea(painter);
    }
    painter.restore();

    if (mode_ == "light" || mode_ == "static-lights") {
        DrawLightSource(painter);
    }

    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(drawing_rect);
}

void MainWindow::DrawLightSource(QPainter& painter) const {
    painter.save();

    const QRect drawing_rect = drawing_area_->geometry();
    const std::vector<QPointF> lights = GetLights();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(216, 140, 0));

    for (const QPointF light : lights) {
        const QPointF light_pos_rel = light;
        const QPointF light_pos_abs = drawing_rect.topLeft() + light_pos_rel;
        painter.setBrush(QColor(216, 140, 0));
        painter.drawEllipse(light_pos_abs, 3, 3);
    }

    painter.setBrush(QColor(216, 140, 0));

    for (const auto& light : static_lights_) {
        painter.drawEllipse(drawing_rect.topLeft() + light, 4, 4);
    }
    painter.restore();
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    const QRect drawing_rect = drawing_area_->geometry();
    if (!drawing_rect.contains(event->pos())) {
        return;
    }
    const QPointF adjusted_pos = event->pos() - drawing_rect.topLeft();
    if (mode_ == "static-lights" && event->button() == Qt::LeftButton) {
        static_lights_.push_back(adjusted_pos);
    } else if (mode_ == "static-lights" && event->button() == Qt::RightButton) {
        const QPointF pos = event->pos() - drawing_rect.topLeft();
        static_lights_.erase(
            std::ranges::remove_if(
                static_lights_,
                [pos, this](const QPointF& p) { return Controller::Distance(p, pos) < 8.0; })
                .begin(),
            static_lights_.end());
        update();
    }
    if (mode_ == "polygons") {
        const auto& polygons = controller_.GetPolygons();
        if (event->button() == Qt::LeftButton) {
            if (!creating_polygon_) {
                controller_.AddPolygon(Polygon({adjusted_pos}));
                creating_polygon_ = true;
            } else {
                if (polygons.empty()) {
                    return;
                }
                controller_.AddVertexToLastPolygon(adjusted_pos);
            }
        } else if (event->button() == Qt::RightButton) {
            if (!polygons.empty() && polygons.back().GetVertices().size() < 2) {
                controller_.GetPolygons().pop_back();
            }
            creating_polygon_ = false;
        }
    }

    else if (mode_ == "light") {
        const QPointF old_light = controller_.GetLightSource();
        const bool can_move = InLight(GetLights(), controller_.GetPolygons());
        if (controller_.IsPositionValid(adjusted_pos)) {
            controller_.SetLightSource(adjusted_pos);
            if (InLight(GetLights(), controller_.GetPolygons()) && !can_move) {
                controller_.SetLightSource(old_light);
            }
        } else {
            controller_.SetLightSource(old_light);
        }
    }
    update();
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    if (mode_ == "light") {
        const QRect drawing_rect = drawing_area_->geometry();
        if (drawing_rect.contains(event->pos())) {
            const QPointF adjusted_pos = event->pos() - drawing_rect.topLeft();
            const QPointF old_light = controller_.GetLightSource();
            const bool can_move = InLight(GetLights(), controller_.GetPolygons());
            if (controller_.IsPositionValid(adjusted_pos)) {
                controller_.SetLightSource(adjusted_pos);
                if (InLight(GetLights(), controller_.GetPolygons()) && !can_move) {
                    controller_.SetLightSource(old_light);
                }
            } else {
                controller_.SetLightSource(old_light);
            }
            update();
        }
    } else if (mode_ == "polygons" && creating_polygon_) {
        if (controller_.IsPositionValid(event->pos() - drawing_area_->geometry().topLeft())) {
            controller_.UpdateLastPolygon(event->pos() - drawing_area_->geometry().topLeft());
        }
        update();
    }
}

std::vector<QPointF> MainWindow::GetLights() const {
    std::vector<QPointF> lights;
    lights.push_back(controller_.GetLightSource());
    constexpr int kCount = 6;
    for (int i = 0; i < kCount; ++i) {
        constexpr int kRadius = 16;
        const double angle = 2 * M_PI * i / kCount;
        lights.emplace_back(
            lights[0].x() + (kRadius * cosl(angle)), lights[0].y() + (kRadius * sinl(angle)));
    }

    return lights;
}

void MainWindow::DrawLightArea(QPainter& painter) {
    if (mode_ != "light" && mode_ != "static-lights") {
        return;
    }
    const QPointF original_light = controller_.GetLightSource();
    const std::vector<QPointF> points = GetLights();
    const std::vector border = {
                                QPointF(1, 1), QPointF(this->width() - 1, 1),
                                QPointF(this->width() - 1, this->height() - 65 - 1), QPointF(1, this->height() - 65 - 1)};

    for (auto point : points) {
        if (!IsPointInPolygon(point, border)) {
            continue;
        }
        controller_.SetLightSource(point);
        const auto rays = controller_.CastRays();
        const Polygon light_area = controller_.CreateLightArea();
        const auto& vertices = light_area.GetVertices();

        if (vertices.size() > 2) {
            QPainterPath path;
            path.moveTo(vertices[0]);
            for (size_t j = 1; j < vertices.size(); ++j) {
                path.lineTo(vertices[j]);
            }
            path.closeSubpath();

            painter.setPen(QPen(Qt::NoPen));
            painter.setBrush(QColor(255, 255, 255, 80));
            painter.drawPath(path);
        }
    }
    for (auto point : static_lights_) {
        controller_.SetLightSource(point);
        const auto rays = controller_.CastRays();
        const Polygon light_area = controller_.CreateLightArea();
        const auto& vertices = light_area.GetVertices();

        if (vertices.size() > 2) {
            QPainterPath path;
            path.moveTo(vertices[0]);
            for (size_t j = 1; j < vertices.size(); ++j) {
                path.lineTo(vertices[j]);
            }
            path.closeSubpath();

            QRadialGradient gradient(point, 250.0);
            gradient.setColorAt(0, QColor(255, 240, 150, 150));
            gradient.setColorAt(1, QColor(255, 240, 150, 0));

            painter.setPen(QPen(Qt::NoPen));
            painter.setBrush(gradient);
            painter.drawPath(path);
        }
    }
    controller_.SetLightSource(original_light);
}

void MainWindow::DrawPolygons(QPainter& painter) const {
    QBrush polygonBrush(Qt::NoBrush);
    QPen polygonPen(Qt::white);
    painter.setBrush(polygonBrush);
    painter.setPen(polygonPen);

    const auto& polygons = controller_.GetPolygons();
    for (size_t i = 0; i < polygons.size(); ++i) {
        const auto& vertices = polygons[i].GetVertices();
        if (vertices.size() < 2) {
            continue;
        }
        QPolygonF poly;
        for (const auto& v : vertices) {
            poly << v;
        }

        if (i == 0) {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
        } else {
            painter.setBrush(polygonBrush);
            painter.setPen(polygonPen);
        }

        painter.drawPolygon(poly);

        if (creating_polygon_ && i == polygons.size() - 1) {
            painter.setBrush(QBrush(QColor(0,0,0)));
            painter.drawPolygon(poly);
        }
    }
}

bool MainWindow::IsPointInPolygon(const QPointF& point, const std::vector<QPointF>& polygon) {
    if (polygon.size() < 3) {
        return false;
    }
    bool inside = false;
    for (size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
        if ((polygon[i].y() > point.y()) != (polygon[j].y() > point.y()) &&
            point.x() < (polygon[j].x() - polygon[i].x()) * (point.y() - polygon[i].y()) /
                                (polygon[j].y() - polygon[i].y()) +
                            polygon[i].x()) {
            inside = !inside;
        }
    }
    return inside;
}

bool MainWindow::InLight(
    const std::vector<QPointF>& light_pos, const std::vector<Polygon>& polygon) {
    bool light_in = false;
    for (const auto& point : light_pos) {
        for (int i = 1; i < polygon.size(); ++i) {
            if (IsPointInPolygon(point, polygon[i].GetVertices())) {
                light_in = true;
            }
        }
    }
    return light_in;
}
