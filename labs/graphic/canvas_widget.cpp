#include "canvas_widget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QPolygonF>

namespace {
QPolygonF ToQPolygonF(const std::vector<QPointF>& vertices) {
    QPolygonF polygon;
    for (const QPointF& vertex : vertices) {
        polygon << vertex;
    }
    return polygon;
}
}

CanvasWidget::CanvasWidget(QWidget* parent)
    : QWidget(parent), mode_(Mode::Light), drawing_polygon_(false) {
    setMouseTracking(true);
    setMinimumSize(800, 600);
    controller_.SetSceneSize(size());
}

Controller& CanvasWidget::GetController() {
    return controller_;
}

void CanvasWidget::SetMode(Mode mode) {
    mode_ = mode;
    update();
}

CanvasWidget::Mode CanvasWidget::GetMode() const {
    return mode_;
}

void CanvasWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor(16, 16, 20));

    DrawLightAreas(&painter);
    DrawPolygons(&painter);
    DrawLightSources(&painter);
}

void CanvasWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    controller_.SetSceneSize(event->size());
}

void CanvasWidget::mouseMoveEvent(QMouseEvent* event) {
    if (mode_ == Mode::Light) {
        controller_.SetLightSource(event->position());
        update();
        return;
    }

    if (mode_ == Mode::Polygons && drawing_polygon_) {
        controller_.UpdateLastPolygon(event->position());
        update();
    }
}

void CanvasWidget::mousePressEvent(QMouseEvent* event) {
    if (mode_ == Mode::Light) {
        controller_.SetLightSource(event->position());
        update();
        return;
    }

    if (mode_ == Mode::Polygons) {
        if (event->button() == Qt::LeftButton) {
            const QPointF point = event->position();
            if (!drawing_polygon_) {
                controller_.AddPolygon(Polygon(std::vector<QPointF>{point, point}));
                drawing_polygon_ = true;
            } else {
                controller_.UpdateLastPolygon(point);
                controller_.AddVertexToLastPolygon(point);
            }
            update();
            return;
        }

        if (event->button() == Qt::RightButton && drawing_polygon_) {
            controller_.FinalizeLastPolygon();
            drawing_polygon_ = false;
            update();
        }
    }
}

void CanvasWidget::DrawLightAreas(QPainter* painter) {
    painter->setPen(Qt::NoPen);
    for (const QPointF& source : controller_.GetLightSources()) {
        const Polygon light_area = controller_.CreateLightAreaForSource(source);
        const std::vector<QPointF>& vertices = light_area.GetVertices();
        if (vertices.size() < 3) {
            continue;
        }
        painter->setBrush(QColor(255, 244, 180, 32));
        painter->drawPolygon(ToQPolygonF(vertices));
    }
}

void CanvasWidget::DrawPolygons(QPainter* painter) {
    const std::vector<Polygon>& polygons = controller_.GetPolygons();

    for (std::size_t i = 0; i < polygons.size(); ++i) {
        const std::vector<QPointF>& vertices = polygons[i].GetVertices();
        if (vertices.empty()) {
            continue;
        }

        const bool active = drawing_polygon_ && i + 1 == polygons.size();
        const QPolygonF polygon = ToQPolygonF(vertices);

        if (!active && vertices.size() >= 3) {
            painter->setPen(QPen(QColor(210, 210, 220), 2.0));
            painter->setBrush(QColor(45, 45, 55));
            painter->drawPolygon(polygon);
        } else {
            painter->setPen(QPen(QColor(210, 210, 220), 2.0));
            painter->setBrush(Qt::NoBrush);
            painter->drawPolyline(polygon);
            for (const QPointF& vertex : vertices) {
                painter->setBrush(QColor(240, 240, 245));
                painter->drawEllipse(vertex, 3.0, 3.0);
                painter->setBrush(Qt::NoBrush);
            }
        }
    }
}

void CanvasWidget::DrawLightSources(QPainter* painter) {
    painter->setPen(Qt::NoPen);
    for (const QPointF& source : controller_.GetLightSources()) {
        painter->setBrush(QColor(255, 250, 210, 200));
        painter->drawEllipse(source, 4.0, 4.0);
    }

    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(QColor(255, 235, 150), 1.5));
    painter->drawEllipse(controller_.GetLightSource(), 8.0, 8.0);
}
