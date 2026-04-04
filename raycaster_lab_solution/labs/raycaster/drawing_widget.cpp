#include "labs/raycaster/drawing_widget.h"

#include <QtCore/QVector>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QPen>
#include <QtWidgets/QStyleOption>

namespace raycaster {

DrawingWidget::DrawingWidget(QWidget* parent)
    : QWidget(parent) {
    setMouseTracking(true);
    setAutoFillBackground(false);
    controller_.SetSceneRect(rect());
}

Controller* DrawingWidget::GetController() {
    return &controller_;
}

const Controller* DrawingWidget::GetController() const {
    return &controller_;
}

void DrawingWidget::SetMode(Mode mode) {
    mode_ = mode;
    update();
}

void DrawingWidget::paintEvent(QPaintEvent* /*event*/) {
    QStyleOption option;
    option.initFrom(this);

    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor(10, 10, 16));

    const auto light_areas = controller_.CreateSoftLightAreas();
    const QColor light_color(255, 245, 180, 55);
    const QColor light_outline(255, 245, 180, 100);
    for (const Polygon& area : light_areas) {
        DrawPolygon(&painter, area, light_color, light_outline);
    }

    painter.setPen(QPen(QColor(255, 255, 255, 30), 1.0, Qt::DashLine));
    if (controller_.GetCurrentPolygon().has_value()) {
        DrawPolygon(&painter, *controller_.GetCurrentPolygon(),
                    QColor(90, 170, 255, 35), QColor(90, 170, 255, 210));
    }

    for (const Polygon& polygon : controller_.GetPolygons()) {
        DrawPolygon(&painter, polygon, QColor(40, 40, 55), QColor(220, 220, 235));
    }

    const auto sources = controller_.GetLightSources();
    for (std::size_t index = 0; index < sources.size(); ++index) {
        DrawLightSource(
            &painter,
            sources[index],
            index == 0U ? QColor(255, 240, 120) : QColor(255, 240, 120, 170));
    }
}

void DrawingWidget::mouseMoveEvent(QMouseEvent* event) {
    const QPointF position = event->position();
    if (mode_ == Mode::kLight) {
        controller_.SetLightSource(position);
    } else if (controller_.HasCurrentPolygon()) {
        controller_.UpdateLastPolygon(position);
    }

    update();
    QWidget::mouseMoveEvent(event);
}

void DrawingWidget::mousePressEvent(QMouseEvent* event) {
    const QPointF position = event->position();
    if (mode_ == Mode::kLight) {
        controller_.SetLightSource(position);
        update();
        QWidget::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        if (!controller_.HasCurrentPolygon()) {
            controller_.AddPolygon(Polygon({position, position}));
        } else {
            controller_.UpdateLastPolygon(position);
            controller_.AddVertexToLastPolygon(position);
        }
    } else if (event->button() == Qt::RightButton) {
        controller_.FinalizeLastPolygon();
    }

    update();
    QWidget::mousePressEvent(event);
}

void DrawingWidget::resizeEvent(QResizeEvent* event) {
    controller_.SetSceneRect(rect());
    QWidget::resizeEvent(event);
}

void DrawingWidget::DrawPolygon(
    QPainter* painter,
    const Polygon& polygon,
    const QColor& fill_color,
    const QColor& outline_color) const {
    const auto& vertices = polygon.GetVertices();
    if (vertices.empty()) {
        return;
    }

    QPainterPath path;
    path.moveTo(vertices.front());
    for (std::size_t index = 1; index < vertices.size(); ++index) {
        path.lineTo(vertices[index]);
    }

    if (vertices.size() >= 3U) {
        path.closeSubpath();
    }

    painter->save();
    painter->setPen(QPen(outline_color, 1.6));
    painter->setBrush(fill_color);
    painter->drawPath(path);

    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(outline_color, 5.0));
    for (const QPointF& vertex : vertices) {
        painter->drawPoint(vertex);
    }
    painter->restore();
}

void DrawingWidget::DrawLightSource(
    QPainter* painter,
    const QPointF& point,
    const QColor& color) const {
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->drawEllipse(point, 4.5, 4.5);
    painter->restore();
}

}  // namespace raycaster
