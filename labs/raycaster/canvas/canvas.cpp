#include "canvas.h"

#include "controller.h"

#include <QMouseEvent>
#include <QPainterPath>
#include <QPen>
#include <QTimer>
#include <cmath>
#include <memory>

namespace {
void DrawLightArea(QPainter& painter, Controller* controller) {
    auto light_area = controller->CreateLightArea();
    for (const auto& [light_position, rays, light_type] : light_area) {
        QPainterPath path;
        const auto& vertices = rays.GetVertices();
        if (vertices.empty()) {
            continue;
        }
        path.moveTo(light_position);
        for (const auto& vertex : vertices) {
            path.lineTo(vertex);
        }
        path.closeSubpath();
        painter.setPen(Qt::NoPen);
        if (light_type == light_type::main_light) {
            painter.setBrush(QColor(255, 215, 0, 80));
        } else if (light_type == light_type::satellite_light) {
            painter.setBrush(QColor(255, 215, 0, 30));
        } else {
            painter.setBrush(QColor(0, 255, 255, 80));
        }
        painter.drawPath(path);
    }
}

void DrawPolygons(QPainter& painter, Controller* controller) {
    painter.setPen(QPen(Qt::darkBlue, 2));
    painter.setBrush(QColor(100, 150, 255, 50));
    const auto& polygons = controller->GetPolygons();

    for (const auto& polygon : polygons) {
        const auto& vertices = polygon.GetVertices();
        if (vertices.size() < 2) {
            continue;
        }
        for (size_t i = 0; i < vertices.size() - 1; ++i) {
            painter.drawLine(vertices[i], vertices[i + 1]);
        }
        painter.drawLine(vertices.back(), vertices.front());
    }

    if (!controller->IsComplete() && !polygons.empty()) {
        const auto& last_polygon = polygons.back();
        if (!last_polygon.GetVertices().empty()) {
            const auto& vertices = last_polygon.GetVertices();
            const auto& last_vertex = vertices.back();
            painter.setPen(QPen(Qt::red, 2));
            painter.setBrush(Qt::red);
            painter.drawEllipse(last_vertex, 5, 5);
        }
    }
}

void DrawLights(QPainter& painter, Controller* controller) {
    const auto& light_position = controller->GetLightSource();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::red);
    painter.drawEllipse(light_position, 8, 8);
    for (const auto& static_light : controller->GetStaticLights()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::red);
        painter.drawEllipse(static_light, 5, 5);
    }
}
};  // namespace

void Canvas::paintEvent(QPaintEvent* event) {
    frame_count_++;
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setRenderHints(QPainter::TextAntialiasing);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);

    DrawLightArea(painter, controller_.get());
    DrawPolygons(painter, controller_.get());
    DrawLights(painter, controller_.get());
}

int Canvas::TakeFrameCount() {
    int count = frame_count_;
    frame_count_ = 0;
    return count;
}

Canvas::Canvas(QWidget* parent) : QWidget(parent) {
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    controller_ = std::make_unique<Controller>();
    SetMode(std::make_unique<LightMode>());
    QTimer::singleShot(0, this, [this]() { UpdateBorder(contentsRect()); });
}

void Canvas::SetMode(std::unique_ptr<CanvasMode> mode) {
    current_mode_ = std::move(mode);
}

void Canvas::SetController(std::unique_ptr<Controller> controller) {
    controller_ = std::move(controller);
}

Controller* Canvas::GetController() const {
    return controller_.get();
}

void Canvas::UpdateBorder(const QRect& rect) {
    Polygon border_polygon;
    border_polygon.AddVertex(QPointF(rect.left(), rect.top()));
    border_polygon.AddVertex(QPointF(rect.right(), rect.top()));
    border_polygon.AddVertex(QPointF(rect.right(), rect.bottom()));
    border_polygon.AddVertex(QPointF(rect.left(), rect.bottom()));
    border_polygon.AddVertex(QPointF(rect.left(), rect.top()));

    auto& polygons = controller_->GetPolygons();

    if (polygons.empty()) {
        controller_->SetLightSource(rect.center());
        polygons.emplace_back(border_polygon);
    } else {
        polygons.at(0) = border_polygon;
    }
}

void Canvas::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    UpdateBorder(contentsRect());
}

void Canvas::ResetCanvas() {
    controller_->GetPolygons().resize(1);
    controller_->GetStaticLights().clear();
    controller_->SetComplete(true);
    controller_->SetDragging(false);
    controller_->SetLightSource(contentsRect().center());
    this->update();
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    if (current_mode_) {
        current_mode_->mousePressEvent(event, this);
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    if (current_mode_) {
        current_mode_->mouseMoveEvent(event, this);
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
    if (current_mode_) {
        current_mode_->mouseReleaseEvent(event, this);
    }
}

void LightMode::mousePressEvent(QMouseEvent* event, Canvas* canvas) {
    if (event->button() == Qt::LeftButton) {
        auto* controller = canvas->GetController();
        controller->SetDragging(true);
        canvas->update();
    }
}

void LightMode::mouseMoveEvent(QMouseEvent* event, Canvas* canvas) {
    auto* controller = canvas->GetController();
    if (controller->IsDragging()) {
        controller->SetLightSource(event->pos());
        canvas->update();
    }
}

void LightMode::mouseReleaseEvent(QMouseEvent* event, Canvas* canvas) {
    if (event->button() == Qt::LeftButton) {
        auto* controller = canvas->GetController();
        controller->SetDragging(false);
    }
}

void PolygonMode::mousePressEvent(QMouseEvent* event, Canvas* canvas) {
    auto* controller = canvas->GetController();
    switch (event->button()) {
        case Qt::LeftButton:
            if (controller->IsComplete()) {
                controller->AddPolygon(Polygon());
                controller->SetComplete(false);
            }
            controller->AddVertexToLastPolygon(event->pos());
            break;
        case Qt::RightButton:
            controller->FinishPolygon();
            break;
        default:
            return;
    }
    canvas->update();
}

void PolygonMode::mouseMoveEvent(QMouseEvent* event, Canvas* canvas) {
    auto* controller = canvas->GetController();
    if (controller->GetPolygons().empty()) {
        return;
    }
    if (!controller->GetPolygons().back().GetVertices().empty()) {
        controller->UpdateLastPolygon(event->pos());
        canvas->update();
    }
}

void PolygonMode::mouseReleaseEvent(QMouseEvent* event, Canvas* canvas) {
    Q_UNUSED(event);
    Q_UNUSED(canvas);
}

void StaticLightsMode::mousePressEvent(QMouseEvent* event, Canvas* canvas) {
    if (event->button() == Qt::LeftButton) {
        auto* controller = canvas->GetController();
        controller->AddStaticLight(event->pos());
        controller->SetDragging(true);
        canvas->update();
    }
}

void StaticLightsMode::mouseMoveEvent(QMouseEvent* event, Canvas* canvas) {
    auto* controller = canvas->GetController();
    if (controller->IsDragging()) {
        controller->UpdateLastStaticLight(event->pos());
        canvas->update();
    }
}

void StaticLightsMode::mouseReleaseEvent(QMouseEvent* event, Canvas* canvas) {
    if (event->button() == Qt::LeftButton) {
        auto* controller = canvas->GetController();
        controller->SetDragging(false);
    }
}
