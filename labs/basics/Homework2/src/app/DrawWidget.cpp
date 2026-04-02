#include "app/DrawWidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>

#include "ui/RenderConfig.h"

DrawWidget::DrawWidget(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true);
    controller_.SetLightSource(ClampClusterCenter(rect().center()));
    controller_.SetLightOffsets({
        QPointF(0.0, 0.0),

        QPointF(8.0, 0.0),
        QPointF(6.5, 4.7),
        QPointF(2.5, 7.6),
        QPointF(-2.5, 7.6),
        QPointF(-6.5, 4.7),
        QPointF(-8.0, 0.0),
        QPointF(-6.5, -4.7),
        QPointF(-2.5, -7.6),
        QPointF(2.5, -7.6),
        QPointF(6.5, -4.7),
    });
    UpdateBoundaryPolygon();
}

void DrawWidget::SetMode(InteractionMode mode) {
    mode_ = mode;
    update();
}

InteractionMode DrawWidget::GetMode() const {
    return mode_;
}

void DrawWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    DrawBackground(painter);
    DrawLightArea(painter);
    DrawPolygons(painter);
    DrawLightSource(painter);
}

void DrawWidget::mousePressEvent(QMouseEvent* event) {
    const QPointF pos = event->position();

    if (mode_ == InteractionMode::Light) {
        if (event->button() == Qt::RightButton) {
            is_light_following_cursor_ = !is_light_following_cursor_;
        } else if (event->button() == Qt::LeftButton) {
            QPointF new_pos = ClampClusterCenter(pos);
            if (controller_.IsLightSourcePositionValid(new_pos)) {
                controller_.SetLightSource(new_pos);
            }
        }

        update();
        return;
    }

    if (mode_ == InteractionMode::StaticLights) {
        if (event->button() == Qt::RightButton) {
            is_light_following_cursor_ = !is_light_following_cursor_;
        } else if (event->button() == Qt::LeftButton) {
            const QPointF current_dynamic = controller_.GetLightSource();
            if (controller_.IsLightSourcePositionValid(current_dynamic)) {
                controller_.AddStaticLightSource(current_dynamic);
            }

            QPointF new_pos = ClampClusterCenter(pos);
            if (controller_.IsLightSourcePositionValid(new_pos)) {
                controller_.SetLightSource(new_pos);
            }

            is_light_following_cursor_ = true;
        }

        update();
        return;
    }

    if (mode_ == InteractionMode::Polygons) {
        if (event->button() == Qt::LeftButton) {
            if (!is_drawing_polygon_) {
                controller_.AddPolygon(Polygon({pos, pos}));
                is_drawing_polygon_ = true;
            } else {
                Polygon candidate = controller_.GetLastPolygon();
                candidate.UpdateLastVertex(pos);

                int current_index = static_cast<int>(controller_.PolygonCount()) - 1;
                if (controller_.CanPlacePolygon(candidate, current_index, true)) {
                    controller_.UpdateLastPolygon(pos);
                    controller_.AddVertexToLastPolygon(pos);
                }
            }
        } else if (event->button() == Qt::RightButton) {
            if (is_drawing_polygon_) {
                controller_.RemoveLastVertexFromLastPolygon();

                if (controller_.GetLastPolygon().VertexCount() < 3) {
                    controller_.RemoveLastPolygon();
                } else {
                    int current_index = static_cast<int>(controller_.PolygonCount()) - 1;
                    if (!controller_.CanPlacePolygon(controller_.GetLastPolygon(), current_index, false)) {
                        controller_.RemoveLastPolygon();
                    }
                }

                is_drawing_polygon_ = false;
            }
        }

        update();
        return;
    }
}

void DrawWidget::mouseMoveEvent(QMouseEvent* event) {
    const QPointF pos = event->position();

    if (mode_ == InteractionMode::Light || mode_ == InteractionMode::StaticLights) {
        if (is_light_following_cursor_) {
            QPointF new_pos = ClampClusterCenter(pos);
            if (controller_.IsLightSourcePositionValid(new_pos)) {
                controller_.SetLightSource(new_pos);
                update();
            }
        }
        return;
    }

    if (mode_ == InteractionMode::Polygons && is_drawing_polygon_) {
        Polygon candidate = controller_.GetLastPolygon();
        candidate.UpdateLastVertex(pos);

        int current_index = static_cast<int>(controller_.PolygonCount()) - 1;
        if (controller_.CanPlacePolygon(candidate, current_index, true)) {
            controller_.UpdateLastPolygon(pos);
            update();
        }
    }
}

void DrawWidget::resizeEvent(QResizeEvent* /*event*/) {
    UpdateBoundaryPolygon();
    controller_.SetLightSource(ClampClusterCenter(controller_.GetLightSource()));
    update();
}

Polygon DrawWidget::MakeBoundaryPolygon() const {
    const int m = render_config::kBoundaryMargin;
    return Polygon({
        QPointF(m, m),
        QPointF(width() - m, m),
        QPointF(width() - m, height() - m),
        QPointF(m, height() - m),
    });
}

void DrawWidget::UpdateBoundaryPolygon() {
    Polygon boundary = MakeBoundaryPolygon();

    if (controller_.Empty()) {
        controller_.AddPolygon(boundary);
    } else {
        controller_.SetPolygon(0, boundary);
    }
}

QPointF DrawWidget::ClampLightPosition(const QPointF& pos) const {
    const qreal margin = render_config::kLightSourceWallMargin;

    qreal x = pos.x();
    qreal y = pos.y();

    if (x < margin) {
        x = margin;
    }
    if (y < margin) {
        y = margin;
    }
    if (x > width() - margin) {
        x = width() - margin;
    }
    if (y > height() - margin) {
        y = height() - margin;
    }

    return QPointF(x, y);
}

QPointF DrawWidget::ClampClusterCenter(const QPointF& pos) const {
    const double wall_margin = render_config::kLightSourceWallMargin;

    double max_offset_x = 0.0;
    double max_offset_y = 0.0;

    for (const auto& offset : controller_.GetLightOffsets()) {
        max_offset_x = std::max(max_offset_x, std::abs(offset.x()));
        max_offset_y = std::max(max_offset_y, std::abs(offset.y()));
    }

    double x = pos.x();
    double y = pos.y();

    const double min_x = wall_margin + max_offset_x;
    const double max_x = width() - wall_margin - max_offset_x;
    const double min_y = wall_margin + max_offset_y;
    const double max_y = height() - wall_margin - max_offset_y;

    if (x < min_x) {
        x = min_x;
    }
    if (x > max_x) {
        x = max_x;
    }
    if (y < min_y) {
        y = min_y;
    }
    if (y > max_y) {
        y = max_y;
    }

    return QPointF(x, y);
}


void DrawWidget::DrawBackground(QPainter& painter) const {
    painter.fillRect(rect(), render_config::kBackgroundColor);
}

void DrawWidget::DrawLightArea(QPainter& painter) const {
    painter.setPen(Qt::NoPen);

    const std::vector<QPointF> dynamic_lights = controller_.GetAllLightSources();
    for (const auto& light_source : dynamic_lights) {
        Polygon light_area = controller_.CreateLightArea(light_source);
        const auto& vertices = light_area.GetVertices();

        if (vertices.size() < 3) {
            continue;
        }

        QPainterPath path;
        path.moveTo(vertices[0]);
        for (size_t i = 1; i < vertices.size(); ++i) {
            path.lineTo(vertices[i]);
        }
        path.closeSubpath();

        painter.setBrush(render_config::kLightAreaColor);
        painter.drawPath(path);
    }

    const std::vector<QPointF> static_lights = controller_.GetAllStaticLightPoints();
    for (const auto& light_source : static_lights) {
        Polygon light_area = controller_.CreateLightArea(light_source);
        const auto& vertices = light_area.GetVertices();

        if (vertices.size() < 3) {
            continue;
        }

        QPainterPath path;
        path.moveTo(vertices[0]);
        for (size_t i = 1; i < vertices.size(); ++i) {
            path.lineTo(vertices[i]);
        }
        path.closeSubpath();

        painter.setBrush(render_config::kStaticLightAreaColor);
        painter.drawPath(path);
    }
}



void DrawWidget::DrawPolygons(QPainter& painter) const {
    const auto& polygons = controller_.GetPolygons();

    for (size_t i = 0; i < polygons.size(); ++i) {
        if (i == 0) {
            continue;
        }

        const auto& vertices = polygons[i].GetVertices();
        if (vertices.empty()) {
            continue;
        }

        QPen edge_pen(render_config::kPolygonEdgeColor);
        edge_pen.setWidthF(render_config::kEdgeWidth);
        painter.setPen(edge_pen);

        for (size_t j = 1; j < vertices.size(); ++j) {
            painter.drawLine(vertices[j - 1], vertices[j]);
        }

        const bool is_preview_polygon = is_drawing_polygon_ && (i + 1 == polygons.size());
        if (!is_preview_polygon && vertices.size() >= 3) {
            painter.drawLine(vertices.back(), vertices.front());
        }

        painter.setPen(Qt::NoPen);
        for (size_t j = 0; j < vertices.size(); ++j) {
            if (is_preview_polygon && j + 1 == vertices.size()) {
                painter.setBrush(render_config::kPreviewColor);
            } else {
                painter.setBrush(render_config::kPolygonVertexColor);
            }

            painter.drawEllipse(
                vertices[j],
                render_config::kVertexRadius,
                render_config::kVertexRadius
            );
        }
    }
}

void DrawWidget::DrawLightSource(QPainter& painter) const {
    painter.setPen(Qt::NoPen);

    const std::vector<QPointF> dynamic_lights = controller_.GetAllLightSources();
    if (!dynamic_lights.empty()) {
        painter.setBrush(render_config::kMainLightSourceColor);
        painter.drawEllipse(
            dynamic_lights[0],
            render_config::kMainLightSourceRadius,
            render_config::kMainLightSourceRadius
        );

        painter.setBrush(render_config::kSecondaryLightSourceColor);
        for (size_t i = 1; i < dynamic_lights.size(); ++i) {
            painter.drawEllipse(
                dynamic_lights[i],
                render_config::kSecondaryLightSourceRadius,
                render_config::kSecondaryLightSourceRadius
            );
        }
    }

    const std::vector<QPointF> static_points = controller_.GetAllStaticLightPoints();
    if (!static_points.empty()) {
        painter.setBrush(render_config::kStaticSecondaryLightSourceColor);
        for (const auto& p : static_points) {
            painter.drawEllipse(
                p,
                render_config::kStaticSecondaryLightSourceRadius,
                render_config::kStaticSecondaryLightSourceRadius
            );
        }
    }

    const auto& static_centers = controller_.GetStaticLightSources();
    if (!static_centers.empty()) {
        painter.setBrush(render_config::kStaticMainLightSourceColor);
        for (const auto& center : static_centers) {
            painter.drawEllipse(
                center,
                render_config::kStaticMainLightSourceRadius,
                render_config::kStaticMainLightSourceRadius
            );
        }
    }
}
