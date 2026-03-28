#include "../include/raycaster_widget.h"

#include <QBoxLayout>
#include <QGroupBox>
#include <QPainter>
#include <QPainterPath>

RaycasterWidget::RaycasterWidget(QWidget* parent) : QMainWindow(parent) {  // NOLINT
    QWidget* centralWidget = new QWidget(this);                            // NOLINT
    setCentralWidget(centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);              // NOLINT
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget* controlPanel = new QWidget();  // NOLINT//
    controlPanel->setFixedHeight(65);
    CreateModeSelector(controlPanel);

    drawing_area_ = new QWidget();  // NOLINT
    drawing_area_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    drawing_area_->setFocusPolicy(Qt::StrongFocus);

    mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(drawing_area_, 1);

    UpdateBorderPolygon();

    connect(mode_group_, &QButtonGroup::buttonClicked, [this]() { drawing_area_->setFocus(); });

    drawing_area_->setFocus();
}

bool RaycasterWidget::eventFilter(QObject* obj, QEvent* event) {
    if (obj == drawing_area_ && event->type() == QEvent::MouseButtonPress) {
        drawing_area_->setFocus();
        auto* const mouse_event = dynamic_cast<QMouseEvent*>(event);
        mousePressEvent(mouse_event);
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

void RaycasterWidget::OnModeChanged(const int mode) {
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
    if (auto* btn = qobject_cast<QRadioButton*>(focusWidget())) {
        btn->clearFocus();
    }
    update();
}

void RaycasterWidget::BuildWalls(const bool go) {
    wall_ = go;
    if (auto* btn = qobject_cast<QCheckBox*>(focusWidget())) {
        btn->clearFocus();
    }
    update();
}

void RaycasterWidget::CreateModeSelector(QWidget* parent) {
    QGroupBox* mode_box = new QGroupBox("", parent);                             // NOLINT
    QHBoxLayout* radio_layout = new QHBoxLayout;                                 // NOLINT

    light_mode_radio_ = new QRadioButton("Режим света", mode_box);               // NOLINT
    polygons_mode_radio_ = new QRadioButton("Режим многоугольников", mode_box);  // NOLINT
    static_lights_radio_ = new QRadioButton("Статичные источники", mode_box);    // NOLINT
    walls_ = new QCheckBox("Непроходимые фигуры");                               // NOLINT

    light_mode_radio_->setMinimumWidth(120);
    polygons_mode_radio_->setMinimumWidth(180);
    static_lights_radio_->setMinimumWidth(180);
    walls_->setMinimumWidth(180);

    light_mode_radio_->setChecked(true);
    mode_ = "light";
    walls_->setChecked(false);
    wall_ = false;

    mode_group_ = new QButtonGroup(this);  // NOLINT
    mode_group_->addButton(light_mode_radio_, 0);
    mode_group_->addButton(polygons_mode_radio_, 1);
    mode_group_->addButton(static_lights_radio_, 2);

    connect(  // NOLINT
        mode_group_, &QButtonGroup::idClicked, this, &RaycasterWidget::OnModeChanged);

    connect(walls_, &QCheckBox::toggled, this, &RaycasterWidget::BuildWalls);

    radio_layout->addWidget(light_mode_radio_);
    radio_layout->addWidget(polygons_mode_radio_);
    radio_layout->addWidget(static_lights_radio_);
    radio_layout->addStretch();
    radio_layout->addWidget(walls_);
    mode_box->setLayout(radio_layout);

    QVBoxLayout* mainLayout = new QVBoxLayout(parent);  // NOLINT
    mainLayout->addWidget(mode_box);
    mainLayout->setContentsMargins(10, 5, 10, 5);
    mode_box->setStyleSheet(
        "QGroupBox { border: 1px solid gray; border-radius: 3px; margin-top: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; }"
        "QRadioButton { padding: 3px; }");
}

void RaycasterWidget::UpdateBorderPolygon() {
    const std::vector border = {
      QPointF(1, 1), QPointF(this->width() - 1, 1),
      QPointF(this->width() - 1, this->height() - 65 - 1), QPointF(1, this->height() - 65 - 1)};

    if (controller_.GetPolygons().empty()) {
        controller_.AddPolygon(Polygon(border));
    } else {
        controller_.GetPolygons()[0] = Polygon(border);
    }
}

void RaycasterWidget::resizeEvent(QResizeEvent* event) {
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
    for (int i = static_lights_.size() - 1; i >= 0; --i) {
        if (!IsPointInPolygon(static_lights_[i], border)) {
            static_lights_.erase(static_lights_.begin() + i);
        }
    }
    UpdateBorderPolygon();
    update();
}

void RaycasterWidget::paintEvent(QPaintEvent* /*event*/) {
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

void RaycasterWidget::DrawLightSource(QPainter& painter) const {
    painter.save();

    const QRect drawing_rect = drawing_area_->geometry();
    const std::vector<QPointF> lights = GetLights();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 150, 150));

    for (const QPointF light : lights) {
        const QPointF light_pos_rel = light;
        const QPointF light_pos_abs = drawing_rect.topLeft() + light_pos_rel;
        // QRadialGradient gradient(light_pos_abs, 15.0);
        // gradient.setColorAt(0, QColor(237, 226, 200, 200));
        // gradient.setColorAt(1, QColor(137, 110, 45, 100));

        painter.setBrush(QColor(255, 150, 150));
        painter.drawEllipse(light_pos_abs, 3, 3);
    }

    painter.setBrush(QColor(0, 255, 0));

    for (const auto& light : static_lights_) {
        painter.drawEllipse(drawing_rect.topLeft() + light, 4, 4);
    }
    painter.restore();
}

void RaycasterWidget::mousePressEvent(QMouseEvent* event) {  // NOLINT
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
            if (InLight(GetLights(), controller_.GetPolygons()) && !can_move && wall_) {
                controller_.SetLightSource(old_light);
            }
        } else {
            controller_.SetLightSource(old_light);
        }
    }
    update();
}

void RaycasterWidget::mouseMoveEvent(QMouseEvent* event) {
    if (mode_ == "light") {
        const QRect drawing_rect = drawing_area_->geometry();
        if (drawing_rect.contains(event->pos())) {
            const QPointF adjusted_pos = event->pos() - drawing_rect.topLeft();
            const QPointF old_light = controller_.GetLightSource();
            const bool can_move = InLight(GetLights(), controller_.GetPolygons());
            if (controller_.IsPositionValid(adjusted_pos)) {
                controller_.SetLightSource(adjusted_pos);
                if (InLight(GetLights(), controller_.GetPolygons()) && !can_move && wall_) {
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

void RaycasterWidget::keyPressEvent(QKeyEvent* event) {
    if (mode_ == "light") {
        QPointF light = controller_.GetLightSource();
        constexpr int kStep = 5;
        const QPointF old_light = light;
        switch (event->key()) {
            case Qt::Key_Left:
                light.rx() -= kStep;
                break;
            case Qt::Key_Right:
                light.rx() += kStep;
                break;
            case Qt::Key_Up:
                light.ry() -= kStep;
                break;
            case Qt::Key_Down:
                light.ry() += kStep;
                break;
            default:
                return;
        }
        const bool can_move = InLight(GetLights(), controller_.GetPolygons());
        if (controller_.IsPositionValid(light)) {
            controller_.SetLightSource(light);
            if (InLight(GetLights(), controller_.GetPolygons()) && !can_move && wall_) {
                controller_.SetLightSource(old_light);
            }
        } else {
            controller_.SetLightSource(old_light);
        }
        update();
    }
}

std::vector<QPointF> RaycasterWidget::GetLights() const {
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

void RaycasterWidget::DrawLightArea(QPainter& painter) {
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
            painter.setBrush(gradient);  // QColor(255, 240, 150, 70));
            painter.drawPath(path);
        }
    }
    controller_.SetLightSource(original_light);
}

void RaycasterWidget::DrawPolygons(QPainter& painter) const {
    QBrush polygonBrush(Qt::NoBrush);  // NOLINT
    QPen polygonPen(Qt::white, 1.5);   // NOLINT
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
            painter.setBrush(QBrush(QColor(200, 230, 255)));
            painter.drawPolygon(poly);
        }
    }
}

bool RaycasterWidget::IsPointInPolygon(const QPointF& point, const std::vector<QPointF>& polygon) {
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

bool RaycasterWidget::InLight(
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