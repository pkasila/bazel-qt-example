#include "raycaster.h"

#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>

Raycaster::Raycaster(QWidget* parent) : QWidget(parent), controller_(800, 600) {
    setMinimumSize(800, 600);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    mode_combo_ = new QComboBox(this);
    mode_combo_->addItem("Light", static_cast<int>(Mode::Light));
    mode_combo_->addItem("Polygons", static_cast<int>(Mode::Polygons));
    mode_combo_->addItem("Soft Shadows", static_cast<int>(Mode::SoftShadows));
    mode_combo_->addItem("Static Lights", static_cast<int>(Mode::StaticLights));

    connect(
        mode_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &Raycaster::OnModeChanged);

    mode_combo_->setStyleSheet(
        "QComboBox {"
        "  background-color: rgba(30, 30, 30, 180);"
        "  color: white;"
        "  border: 1px solid rgba(255, 255, 255, 50);"
        "  border-radius: 6px;"
        "  padding: 5px 15px;"
        "  font-weight: bold;"
        "}"
        "QComboBox::drop-down {"
        "  border-left: 1px solid rgba(255, 255, 255, 50);"
        "}"
        "QAbstractItemView {"
        "  background-color: rgb(40, 40, 40);"
        "  color: white;"
        "  selection-background-color: rgb(80, 80, 80);"
        "  border-radius: 6px;"
        "}");
    mode_combo_->move(20, 20);
}

void Raycaster::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::black);

    if (mode_ == Mode::Light) {
        DrawLightArea(painter);
        DrawLightSource(painter);
    } else if (mode_ == Mode::SoftShadows) {
        DrawSoftShadows(painter);
        DrawMultipleLightSources(painter);
    } else if (mode_ == Mode::StaticLights) {
        DrawStaticLights(painter);
    }

    DrawPolygons(painter);
    if (mode_ == Mode::Polygons) {
        DrawCurrentPolygon(painter);
    }
}

void Raycaster::mousePressEvent(QMouseEvent* event) {
    QPointF pos = event->position();
    if (mode_ == Mode::Polygons) {
        if (event->button() == Qt::LeftButton) {
            controller_.AddVertexToLastPolygon(pos);
            if (!controller_.HasCurrentPolygon()) {
                controller_.AddVertexToLastPolygon(pos);
            }
            update();
        } else if (event->button() == Qt::RightButton) {
            controller_.RemoveLastVertexFromCurrentPolygon();
            controller_.FinishCurrentPolygon();
            update();
        }
    } else if (mode_ == Mode::StaticLights) {
        if (event->button() == Qt::LeftButton) {
            controller_.AddStaticLight(pos);
            update();
        } else if (event->button() == Qt::RightButton) {
            controller_.RemoveLastStaticLight();
            update();
        }
    }
}

void Raycaster::mouseMoveEvent(QMouseEvent* event) {
    QPointF pos = event->position();
    if (mode_ == Mode::Light || mode_ == Mode::SoftShadows || mode_ == Mode::StaticLights) {
        controller_.SetLightSource(pos);
        update();
    } else if (mode_ == Mode::Polygons && controller_.HasCurrentPolygon()) {
        controller_.UpdateLastPolygon(pos);
        update();
    }
}

void Raycaster::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_C) {
        controller_.ClearAllPolygons();
        update();
        return;
    }

    QPointF pos = controller_.GetLightSource();
    double step = 5.0;
    if (event->key() == Qt::Key_Left) {
        pos.rx() -= step;
    } else if (event->key() == Qt::Key_Right) {
        pos.rx() += step;
    } else if (event->key() == Qt::Key_Up) {
        pos.ry() -= step;
    } else if (event->key() == Qt::Key_Down) {
        pos.ry() += step;
    }

    controller_.SetLightSource(pos);
    update();
}

void Raycaster::resizeEvent(QResizeEvent* event) {
    controller_.UpdateDimensions(event->size().width(), event->size().height());
    mode_combo_->move(20, 20);
    QWidget::resizeEvent(event);
}

void Raycaster::OnModeChanged(int index) {
    mode_ = static_cast<Mode>(mode_combo_->itemData(index).toInt());
    controller_.ClearCurrentPolygon();
    setFocus();
    update();
}

void Raycaster::DrawLightArea(QPainter& painter) {
    Polygon area = controller_.CreateLightArea();
    QPainterPath path;
    const auto& vertices = area.GetVertices();
    if (vertices.empty()) {
        return;
    }

    path.moveTo(vertices[0]);
    for (size_t i = 1; i < vertices.size(); ++i) {
        path.lineTo(vertices[i]);
    }
    path.closeSubpath();

    painter.fillPath(path, QColor(255, 255, 200, 150));
}

void Raycaster::DrawSoftShadows(QPainter& painter) {
    auto areas = controller_.CreateMultipleLightAreas();
    for (const auto& area : areas) {
        QPainterPath path;
        const auto& vertices = area.GetVertices();
        if (vertices.empty()) {
            continue;
        }

        path.moveTo(vertices[0]);
        for (size_t i = 1; i < vertices.size(); ++i) {
            path.lineTo(vertices[i]);
        }
        path.closeSubpath();
        painter.fillPath(path, QColor(255, 255, 255, 30));
    }
}

void Raycaster::DrawStaticLights(QPainter& painter) {
    auto areas = controller_.CreateStaticLightAreas();
    for (const auto& area : areas) {
        QPainterPath path;
        const auto& vertices = area.GetVertices();
        if (vertices.empty()) {
            continue;
        }

        path.moveTo(vertices[0]);
        for (size_t i = 1; i < vertices.size(); ++i) {
            path.lineTo(vertices[i]);
        }
        path.closeSubpath();
        painter.fillPath(path, QColor(255, 255, 100, 100));
    }

    painter.setBrush(Qt::yellow);
    for (const auto& p : controller_.GetStaticLights()) {
        painter.drawEllipse(
            p, static_cast<double>(kStaticLightRadius), static_cast<double>(kStaticLightRadius));
    }
}

void Raycaster::DrawPolygons(QPainter& painter) {
    painter.setPen(Qt::white);
    painter.setBrush(Qt::darkGray);
    const auto& polys = controller_.GetPolygons();
    for (size_t i = 1; i < polys.size(); ++i) {
        QPainterPath path;
        const auto& v = polys[i].GetVertices();
        if (v.empty()) {
            continue;
        }
        path.moveTo(v[0]);
        for (size_t j = 1; j < v.size(); ++j) {
            path.lineTo(v[j]);
        }
        path.closeSubpath();
        painter.drawPath(path);
    }
}

void Raycaster::DrawCurrentPolygon(QPainter& painter) {
    QPen pen(Qt::red);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);

    const auto& v = controller_.GetCurrentPolygonVertices();
    if (v.empty()) {
        return;
    }

    for (size_t i = 0; i < v.size() - 1; ++i) {
        painter.drawLine(v[i], v[i + 1]);
    }

    painter.setPen(Qt::red);
    for (const auto& p : v) {
        painter.drawEllipse(
            p, static_cast<double>(kVertexRadius), static_cast<double>(kVertexRadius));
    }
}

void Raycaster::DrawLightSource(QPainter& painter) {
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(
        controller_.GetLightSource(), static_cast<double>(kLightRadius),
        static_cast<double>(kLightRadius));
}

void Raycaster::DrawMultipleLightSources(QPainter& painter) {
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255, 180));
    for (const auto& src : controller_.GetMultipleLightSources()) {
        painter.drawEllipse(src, 3.0, 3.0);
    }
    painter.setBrush(Qt::white);
    painter.drawEllipse(controller_.GetLightSource(), 4.0, 4.0);
}
