#include "drawing_widget.h"

#include "validator.h"

#include <QPainter>

DrawingWidget::DrawingWidget(QWidget* parent) : QWidget(parent), controller_({}, {}) {
    setMouseTracking(true);
    controller_.SetLightSource(QPoint(400, 300));
}

void DrawingWidget::SetMode(Mode mode) {
    current_mode_ = mode;
    if (is_drawing_polygon_) {
        is_drawing_polygon_ = false;
        if (drawing_state_callback_) drawing_state_callback_(false);
    }
    update();
}

void DrawingWidget::SetLightSource(const QPoint& pos) {
    controller_.SetLightSource(pos);
    update();
}

QPoint DrawingWidget::GetLightSource() const {
    const auto& sources = controller_.GetLightSources();
    return sources.empty() ? QPoint(0, 0) : sources[0];
}

void DrawingWidget::UpdateBoundaryPolygon() {
    std::vector<QPoint> boundary = {
      QPoint(-6, -6), QPoint(width() + 6, -6), QPoint(width() + 6, height() + 6),
      QPoint(-6, height() + 6)};
    if (controller_.GetPolygons().empty()) {
        controller_.AddPolygon(Polygon(boundary));
    } else {
        controller_.ReplacePolygon(0, Polygon(boundary));
    }
}

namespace {

void DrawPolygon(QPainter& painter, const Polygon& polygon) {
    auto vertices = polygon.GetAllVertices();
    if (vertices.size() >= 2) {
        QPolygon qpoly;
        for (const auto& p : vertices) {
            qpoly << p;
        }
        painter.drawPolygon(qpoly);
    }
}

}  // namespace

void DrawingWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QColor(80, 80, 80, 150));
    painter.setBrush(QColor(160, 160, 160, 60));
    for (const auto& poly : controller_.GetPolygons()) {
        DrawPolygon(painter, poly);
    }

    if (current_mode_ == Mode::kLight) {
        const auto& systems = controller_.GetLightSystems();
        for (const auto& system : systems) {
            for (const auto& source : system.light_sources) {
                Polygon light_area = controller_.CreateLightArea(source);
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(255, 255, 180, 30));
                DrawPolygon(painter, light_area);
            }
        }

        if (!systems.empty()) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(255, 255, 150, 200));
            for (const auto& system : systems) {
                painter.drawEllipse(system.light_sources[0], 5, 5);
            }
        }
    }

    if (is_drawing_polygon_) {
        painter.setPen(QColor(100, 100, 255, 150));
        const auto& vertices = controller_.GetPolygons().back().GetAllVertices();
        if (vertices.size() >= 2) {
            for (size_t i = 0; i < vertices.size() - 1; ++i) {
                painter.drawLine(vertices[i], vertices[i + 1]);
            }
        }
    }
}

void DrawingWidget::mousePressEvent(QMouseEvent* event) {
    QPoint pos = event->pos();
    if (current_mode_ == Mode::kPolygons) {
        if (event->button() == Qt::LeftButton) {
            if (!is_drawing_polygon_) {
                is_drawing_polygon_ = true;
                if (drawing_state_callback_) drawing_state_callback_(true);
                controller_.AddPolygon(Polygon({pos, pos}));
            } else {
                auto current_poly = controller_.GetPolygons().back();
                if (Validator::CanAddPolygonVertex(controller_.GetPolygons(), current_poly)) {
                    controller_.AddVertexToLastPolygon(pos);
                }
            }
        } else if (event->button() == Qt::RightButton) {
            auto current_poly = controller_.GetPolygons().back();
            if (is_drawing_polygon_ && Validator::CanAddPolygonVertex(controller_.GetPolygons(), current_poly)) {
                is_drawing_polygon_ = false;
                if (drawing_state_callback_) drawing_state_callback_(false);
            }
        }
    } else if (current_mode_ == Mode::kLight) {
        if (event->button() == Qt::LeftButton) {
            if (Validator::IsLightPositionValid(controller_.GetPolygons(), pos)) {
                controller_.AddStaticLightSource(pos);
            }
        } else if (event->button() == Qt::RightButton) {
            controller_.RemoveLastLightSource();
        }
    }
    update();
}

void DrawingWidget::mouseMoveEvent(QMouseEvent* event) {
    QPoint pos = event->pos();
    if (current_mode_ == Mode::kPolygons && is_drawing_polygon_) {
        controller_.UpdateLastPolygon(pos);
    } else if (current_mode_ == Mode::kLight) {
        if (Validator::IsLightPositionValid(controller_.GetPolygons(), pos)) {
            controller_.SetDynamicLightSource(pos);
        }
    }
    update();
}

void DrawingWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    UpdateBoundaryPolygon();
}
