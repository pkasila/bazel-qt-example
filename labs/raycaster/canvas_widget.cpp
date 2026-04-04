#include "labs/raycaster/canvas_widget.h"

#include <algorithm>

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>

namespace {

void DrawPolyline(QPainter& painter, const std::vector<QPointF>& vertices, bool close_path) {
    if (vertices.size() < 2) {
        return;
    }

    for (std::size_t index = 1; index < vertices.size(); ++index) {
        painter.drawLine(vertices[index - 1], vertices[index]);
    }

    if (close_path && vertices.size() >= 3) {
        painter.drawLine(vertices.back(), vertices.front());
    }
}

}  // namespace

CanvasWidget::CanvasWidget(QWidget* parent)
    : QWidget(parent),
      controller_(),
      mode_(InteractionMode::kLight),
      last_mouse_position_(0.0, 0.0) {
    setMinimumSize(700, 500);
    setMouseTracking(true);
    SyncSceneRect();
}

void CanvasWidget::SetMode(InteractionMode mode) {
    mode_ = mode;
    if (mode_ == InteractionMode::kLight) {
        controller_.SetLightSource(last_mouse_position_);
    } else if (controller_.HasActivePolygon()) {
        controller_.UpdateLastPolygon(last_mouse_position_);
    }
    update();
}

void CanvasWidget::mouseMoveEvent(QMouseEvent* event) {
    last_mouse_position_ = event->position();
    if (mode_ == InteractionMode::kLight) {
        controller_.SetLightSource(last_mouse_position_);
    } else if (controller_.HasActivePolygon()) {
        controller_.UpdateLastPolygon(last_mouse_position_);
    }
    update();
    QWidget::mouseMoveEvent(event);
}

void CanvasWidget::mousePressEvent(QMouseEvent* event) {
    last_mouse_position_ = event->position();
    if (event->button() == Qt::LeftButton) {
        if (mode_ == InteractionMode::kLight) {
            controller_.SetLightSource(last_mouse_position_);
        } else if (!controller_.HasActivePolygon()) {
            controller_.AddPolygon(Polygon({last_mouse_position_, last_mouse_position_}));
        } else {
            controller_.AddVertexToLastPolygon(last_mouse_position_);
        }
        update();
        return;
    }

    if (event->button() == Qt::RightButton && mode_ == InteractionMode::kPolygons) {
        controller_.FinishActivePolygon();
        update();
        return;
    }

    update();
    QWidget::mousePressEvent(event);
}

void CanvasWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor(6, 7, 10));
    painter.setPen(QPen(QColor(42, 46, 54), 1.0));
    painter.drawRect(rect().adjusted(0, 0, -1, -1));

    DrawFinishedPolygonFills(painter);
    DrawLightAreas(painter);
    DrawFinishedPolygonOutlines(painter);
    DrawDraftPolygon(painter);
    DrawLightSources(painter);

    painter.setPen(QColor(220, 224, 232));
    painter.drawText(16, 28, mode_ == InteractionMode::kLight ? "Mode: light" : "Mode: polygons");
    painter.drawText(16, 50, QString("Mouse: (%1, %2)")
                                .arg(last_mouse_position_.x(), 0, 'f', 1)
                                .arg(last_mouse_position_.y(), 0, 'f', 1));
    painter.drawText(
        16,
        72,
        mode_ == InteractionMode::kLight
            ? "Move the mouse to move the light sources"
            : "LMB: add vertex, RMB: finish polygon"
    );
    if (controller_.HasActivePolygon()) {
        painter.setPen(QColor(255, 181, 71));
        painter.drawText(16, 94, "Draft polygon does not cast shadows yet. Finish it with RMB.");
    }
}

void CanvasWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    SyncSceneRect();
    update();
}

void CanvasWidget::DrawDraftPolygon(QPainter& painter) const {
    if (!controller_.HasActivePolygon()) {
        return;
    }

    const std::size_t active_index = controller_.GetActivePolygonIndex();
    const std::vector<Polygon>& polygons = controller_.GetPolygons();
    if (active_index >= polygons.size()) {
        return;
    }

    const Polygon& polygon = polygons[active_index];
    const std::vector<QPointF>& vertices = polygon.GetVertices();
    if (vertices.empty()) {
        return;
    }

    painter.save();
    painter.setPen(QPen(QColor(255, 181, 71), 2.0, Qt::DashLine));
    painter.setBrush(QColor(255, 181, 71));
    DrawPolyline(painter, vertices, false);
    for (const QPointF& vertex : vertices) {
        painter.drawEllipse(vertex, 3.0, 3.0);
    }
    painter.restore();
}

void CanvasWidget::DrawFinishedPolygonFills(QPainter& painter) const {
    painter.save();
    painter.setBrush(QColor(24, 27, 34));

    const std::vector<Polygon>& polygons = controller_.GetPolygons();
    const bool has_active_polygon = controller_.HasActivePolygon();
    const std::size_t active_index = controller_.GetActivePolygonIndex();
    for (std::size_t index = 0; index < polygons.size(); ++index) {
        if (has_active_polygon && active_index < polygons.size() && index == active_index) {
            continue;
        }

        const std::vector<QPointF>& vertices = polygons[index].GetVertices();
        if (vertices.size() < 3) {
            continue;
        }

        QPainterPath fill_path;
        fill_path.moveTo(vertices.front());
        for (std::size_t vertex_index = 1; vertex_index < vertices.size(); ++vertex_index) {
            fill_path.lineTo(vertices[vertex_index]);
        }
        fill_path.closeSubpath();
        painter.fillPath(fill_path, QColor(24, 27, 34));
    }

    painter.restore();
}

void CanvasWidget::DrawFinishedPolygonOutlines(QPainter& painter) const {
    painter.save();
    painter.setPen(QPen(QColor(116, 126, 146), 2.0));
    painter.setBrush(Qt::NoBrush);

    const std::vector<Polygon>& polygons = controller_.GetPolygons();
    const bool has_active_polygon = controller_.HasActivePolygon();
    const std::size_t active_index = controller_.GetActivePolygonIndex();
    for (std::size_t index = 0; index < polygons.size(); ++index) {
        if (has_active_polygon && active_index < polygons.size() && index == active_index) {
            continue;
        }

        const std::vector<QPointF>& vertices = polygons[index].GetVertices();
        if (vertices.size() < 3) {
            continue;
        }

        DrawPolyline(painter, vertices, true);
    }

    painter.restore();
}

void CanvasWidget::DrawLightAreas(QPainter& painter) const {
    const std::vector<Polygon> light_areas = controller_.CreateLightAreas();

    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setCompositionMode(QPainter::CompositionMode_Plus);

    for (const Polygon& light_area : light_areas) {
        const std::vector<QPointF>& vertices = light_area.GetVertices();
        if (vertices.size() < 3) {
            continue;
        }

        QPainterPath light_path;
        light_path.moveTo(vertices.front());
        for (std::size_t index = 1; index < vertices.size(); ++index) {
            light_path.lineTo(vertices[index]);
        }
        light_path.closeSubpath();
        painter.fillPath(light_path, QColor(255, 255, 245, 44));
    }

    painter.restore();
}

void CanvasWidget::DrawLightSources(QPainter& painter) const {
    const std::vector<QPointF> light_sources = controller_.GetLightSources();

    painter.save();
    painter.setPen(QPen(QColor(255, 250, 214), 1.0));
    painter.setBrush(QColor(255, 245, 200));
    for (const QPointF& source : light_sources) {
        painter.drawEllipse(source, 3.0, 3.0);
    }
    painter.restore();
}

void CanvasWidget::SyncSceneRect() {
    const QRect inner_rect = contentsRect().adjusted(1, 1, -1, -1);
    controller_.SetSceneBounds(inner_rect.topLeft(), inner_rect.bottomRight());
    last_mouse_position_.setX(std::clamp(
        last_mouse_position_.x(),
        static_cast<double>(inner_rect.left()),
        static_cast<double>(inner_rect.right())
    ));
    last_mouse_position_.setY(std::clamp(
        last_mouse_position_.y(),
        static_cast<double>(inner_rect.top()),
        static_cast<double>(inner_rect.bottom())
    ));

    if (mode_ == InteractionMode::kLight) {
        controller_.SetLightSource(last_mouse_position_);
    } else if (controller_.HasActivePolygon()) {
        controller_.UpdateLastPolygon(last_mouse_position_);
    }
}
