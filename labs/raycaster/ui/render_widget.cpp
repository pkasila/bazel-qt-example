#include "labs/raycaster/ui/render_widget.h"

#include <QBrush>
#include <QColor>
#include <QImage>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPolygonF>
#include <QRadialGradient>
#include <QResizeEvent>
#include <algorithm>

namespace raycaster {

RenderWidget::RenderWidget(QWidget* parent)
    : QWidget(parent)
    , mode_(InteractionMode::kLight)
    , cat_overlay_enabled_(false)
    , cat_background_image_(":/labs/raycaster/assets/ginger_cat.jpg") {
    setMouseTracking(true);
    setMinimumSize(900, 650);
    UpdateCursor();
}

void RenderWidget::SetMode(InteractionMode mode) {
    mode_ = mode;
    UpdateCursor();
    update();
}

InteractionMode RenderWidget::GetMode() const {
    return mode_;
}

void RenderWidget::SetSoftShadowsEnabled(bool enabled) {
    controller_.SetSoftShadowsEnabled(enabled);
    update();
}

bool RenderWidget::AreSoftShadowsEnabled() const {
    return controller_.AreSoftShadowsEnabled();
}

void RenderWidget::SetCatOverlayEnabled(bool enabled) {
    cat_overlay_enabled_ = enabled;
    update();
}

bool RenderWidget::IsCatOverlayEnabled() const {
    return cat_overlay_enabled_;
}

Controller& RenderWidget::GetController() {
    return controller_;
}

const Controller& RenderWidget::GetController() const {
    return controller_;
}

void RenderWidget::SetStatusCallback(std::function<void(const QString&)> callback) {
    status_callback_ = std::move(callback);
}

void RenderWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    DrawBackground(&painter);

    DrawLights(&painter);
    DrawObstacles(&painter);
    DrawLightMarkers(&painter);
    DrawCatOverlay(&painter);
}

void RenderWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    controller_.SetSceneBounds(1.0, 1.0, width() - 2.0, height() - 2.0);
}

void RenderWidget::mouseMoveEvent(QMouseEvent* event) {
    if (mode_ == InteractionMode::kLight) {
        UpdateLightPosition(event->position());
    } else if (mode_ == InteractionMode::kPolygons && controller_.IsDrawingPolygon()) {
        controller_.UpdateLastPolygon(event->position());
        update();
    }

    QWidget::mouseMoveEvent(event);
}

void RenderWidget::mousePressEvent(QMouseEvent* event) {
    const QPointF position = event->position();

    if (mode_ == InteractionMode::kLight) {
        UpdateLightPosition(position);
    } else if (mode_ == InteractionMode::kPolygons) {
        if (event->button() == Qt::LeftButton) {
            if (!controller_.IsDrawingPolygon()) {
                controller_.AddPolygon(Polygon({position, position}));
                ShowStatus("Новый многоугольник начат.");
            } else {
                controller_.UpdateLastPolygon(position);
                controller_.AddVertexToLastPolygon(position);
                ShowStatus("Вершина добавлена.");
            }
            update();
        } else if (event->button() == Qt::RightButton) {
            const bool success = controller_.FinalizeLastPolygon();
            if (success) {
                ShowStatus("Многоугольник завершен.");
            } else {
                ShowStatus(
                    "Многоугольник отменен: он слишком маленький, пересекается с другим или "
                    "накрывает источник.");
            }
            update();
        }
    } else if (mode_ == InteractionMode::kStaticLights) {
        if (event->button() == Qt::LeftButton) {
            const std::size_t previous_count = controller_.GetStaticLights().size();
            controller_.AddStaticLight(position);
            if (controller_.GetStaticLights().size() == previous_count) {
                ShowStatus("Нельзя поставить статический источник внутри препятствия.");
            } else {
                ShowStatus("Статический источник света добавлен.");
            }
            update();
        } else if (event->button() == Qt::RightButton) {
            controller_.ClearStaticLights();
            ShowStatus("Статические источники очищены.");
            update();
        }
    }

    QWidget::mousePressEvent(event);
}

void RenderWidget::leaveEvent(QEvent* event) {
    QWidget::leaveEvent(event);
    update();
}

QPolygonF RenderWidget::ToQPolygonF(const Polygon& polygon) {
    QPolygonF result;
    for (const QPointF& vertex : polygon.GetVertices()) {
        result << vertex;
    }
    return result;
}

void RenderWidget::DrawBackground(QPainter* painter) {
    painter->save();
    const QRectF area = rect();

    if (!cat_overlay_enabled_) {
        QLinearGradient base_gradient(area.topLeft(), area.bottomRight());
        base_gradient.setColorAt(0.0, QColor(22, 28, 52));
        base_gradient.setColorAt(0.45, QColor(18, 18, 34));
        base_gradient.setColorAt(1.0, QColor(52, 22, 30));
        painter->fillRect(area, base_gradient);

        QRadialGradient accent(
            QPointF(width() * 0.3, height() * 0.22), 0.62 * std::max(width(), height()));
        accent.setColorAt(0.0, QColor(255, 178, 82, 28));
        accent.setColorAt(0.5, QColor(255, 130, 64, 10));
        accent.setColorAt(1.0, QColor(255, 130, 64, 0));
        painter->fillRect(area, accent);
        painter->restore();
        return;
    }

    QLinearGradient base_gradient(area.topLeft(), area.bottomRight());
    base_gradient.setColorAt(0.0, QColor(58, 28, 20));
    base_gradient.setColorAt(0.3, QColor(126, 58, 24));
    base_gradient.setColorAt(0.75, QColor(74, 32, 24));
    base_gradient.setColorAt(1.0, QColor(22, 12, 14));
    painter->fillRect(area, base_gradient);

    if (!cat_background_image_.isNull()) {
        const QSize target_size = area.size().toSize();
        const QImage scaled = cat_background_image_.scaled(
            target_size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        const int x_offset = std::max(0, (scaled.width() - target_size.width()) / 2);
        const int y_offset = std::max(0, (scaled.height() - target_size.height()) / 3);
        const QRect source_rect(
            x_offset, std::min(y_offset, std::max(0, scaled.height() - target_size.height())),
            std::min(target_size.width(), scaled.width()),
            std::min(target_size.height(), scaled.height()));

        painter->setOpacity(0.34);
        painter->drawImage(area, scaled, source_rect);
        painter->setOpacity(1.0);
    }

    painter->fillRect(area, QColor(24, 10, 6, 76));
    painter->setCompositionMode(QPainter::CompositionMode_SoftLight);
    QLinearGradient warm_overlay(QPointF(0.0, 0.0), QPointF(width(), height()));
    warm_overlay.setColorAt(0.0, QColor(255, 210, 150, 110));
    warm_overlay.setColorAt(0.35, QColor(255, 150, 74, 82));
    warm_overlay.setColorAt(0.8, QColor(160, 58, 20, 64));
    warm_overlay.setColorAt(1.0, QColor(84, 22, 20, 96));
    painter->fillRect(area, warm_overlay);
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

    painter->setPen(Qt::NoPen);

    QRadialGradient glow(
        QPointF(width() * 0.28, height() * 0.24), 0.55 * std::max(width(), height()));
    glow.setColorAt(0.0, QColor(255, 210, 126, 96));
    glow.setColorAt(0.45, QColor(255, 154, 70, 34));
    glow.setColorAt(1.0, QColor(255, 120, 30, 0));
    painter->fillRect(area, glow);

    painter->save();
    painter->translate(width() * 0.14, height() * 0.12);
    painter->scale(4.6, 4.6);

    painter->setBrush(QColor(255, 210, 120, 18));
    QPainterPath face;
    face.addEllipse(QPointF(55.0, 42.0), 32.0, 29.0);
    painter->drawPath(face);

    QPainterPath left_ear;
    left_ear.moveTo(26.0, 30.0);
    left_ear.lineTo(40.0, 5.0);
    left_ear.lineTo(52.0, 34.0);
    left_ear.closeSubpath();

    QPainterPath right_ear;
    right_ear.moveTo(58.0, 34.0);
    right_ear.lineTo(70.0, 5.0);
    right_ear.lineTo(84.0, 30.0);
    right_ear.closeSubpath();

    painter->drawPath(left_ear);
    painter->drawPath(right_ear);

    painter->setPen(QPen(QColor(110, 56, 24, 34), 2.6, Qt::SolidLine, Qt::RoundCap));
    painter->drawArc(QRectF(34.0, 20.0, 12.0, 12.0), 20 * 16, 120 * 16);
    painter->drawArc(QRectF(64.0, 20.0, 12.0, 12.0), 40 * 16, 120 * 16);
    painter->drawLine(QPointF(39.0, 57.0), QPointF(18.0, 54.0));
    painter->drawLine(QPointF(39.0, 61.0), QPointF(16.0, 63.0));
    painter->drawLine(QPointF(71.0, 57.0), QPointF(92.0, 54.0));
    painter->drawLine(QPointF(71.0, 61.0), QPointF(94.0, 63.0));
    painter->restore();

    painter->setBrush(QColor(255, 210, 120, 18));
    for (int i = -1; i < 6; ++i) {
        const qreal x = (width() * 0.12) + (i * width() * 0.18);
        painter->drawRoundedRect(QRectF(x, -30.0, width() * 0.055, height() + 60.0), 26.0, 26.0);
    }

    const auto draw_paw = [painter](const QPointF& center, double scale, const QColor& color) {
        painter->setBrush(color);
        painter->drawEllipse(center, 13.0 * scale, 10.0 * scale);
        painter->drawEllipse(
            center + QPointF(-10.0 * scale, -11.0 * scale), 4.2 * scale, 6.5 * scale);
        painter->drawEllipse(
            center + QPointF(-3.0 * scale, -15.0 * scale), 4.0 * scale, 6.8 * scale);
        painter->drawEllipse(
            center + QPointF(4.0 * scale, -15.0 * scale), 4.0 * scale, 6.8 * scale);
        painter->drawEllipse(
            center + QPointF(11.0 * scale, -11.0 * scale), 4.2 * scale, 6.5 * scale);
    };

    draw_paw(QPointF(width() * 0.82, height() * 0.2), 1.1, QColor(255, 228, 170, 32));
    draw_paw(QPointF(width() * 0.9, height() * 0.3), 0.9, QColor(255, 228, 170, 26));
    draw_paw(QPointF(width() * 0.76, height() * 0.72), 1.0, QColor(255, 228, 170, 24));

    QLinearGradient vignette(area.topLeft(), area.bottomLeft());
    vignette.setColorAt(0.0, QColor(18, 10, 12, 0));
    vignette.setColorAt(1.0, QColor(10, 8, 12, 92));
    painter->fillRect(area, vignette);

    painter->restore();
}

void RenderWidget::UpdateCursor() {
    switch (mode_) {
        case InteractionMode::kLight:
            setCursor(Qt::BlankCursor);
            break;
        case InteractionMode::kPolygons:
        case InteractionMode::kStaticLights:
            setCursor(Qt::CrossCursor);
            break;
    }
}

void RenderWidget::DrawLightArea(
    QPainter* painter, const QPointF& source, const QColor& center_color, const QColor& outer_color,
    double radius) {
    const Polygon light_area = controller_.CreateLightAreaForSource(source);
    const QPolygonF polygon = ToQPolygonF(light_area);
    if (polygon.size() < 3) {
        return;
    }

    QPainterPath light_path;
    light_path.addPolygon(polygon);
    light_path.closeSubpath();

    QRadialGradient gradient(source, radius);
    gradient.setColorAt(0.0, center_color);
    gradient.setColorAt(
        0.2, QColor(center_color.red(), center_color.green(), center_color.blue(), 120));
    gradient.setColorAt(
        0.65, QColor(outer_color.red(), outer_color.green(), outer_color.blue(), 55));
    gradient.setColorAt(1.0, outer_color);

    painter->fillPath(light_path, gradient);
}

void RenderWidget::DrawCatOverlay(QPainter* painter) {
    if (!cat_overlay_enabled_) {
        return;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(width() - 112.0, height() - 112.0);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 60));
    painter->drawEllipse(QPointF(54.0, 82.0), 34.0, 14.0);

    QPainterPath left_ear;
    left_ear.moveTo(26.0, 30.0);
    left_ear.lineTo(40.0, 5.0);
    left_ear.lineTo(52.0, 34.0);
    left_ear.closeSubpath();

    QPainterPath right_ear;
    right_ear.moveTo(58.0, 34.0);
    right_ear.lineTo(70.0, 5.0);
    right_ear.lineTo(84.0, 30.0);
    right_ear.closeSubpath();

    painter->setBrush(QColor(245, 150, 48, 245));
    painter->drawPath(left_ear);
    painter->drawPath(right_ear);
    painter->drawEllipse(QPointF(55.0, 42.0), 32.0, 29.0);

    painter->setBrush(QColor(255, 208, 150, 235));
    painter->drawEllipse(QPointF(55.0, 52.0), 19.0, 14.0);

    painter->setPen(QPen(QColor(92, 46, 20, 215), 2.2, Qt::SolidLine, Qt::RoundCap));
    painter->drawArc(QRectF(34.0, 20.0, 12.0, 12.0), 20 * 16, 120 * 16);
    painter->drawArc(QRectF(64.0, 20.0, 12.0, 12.0), 40 * 16, 120 * 16);
    painter->drawLine(QPointF(44.0, 44.0), QPointF(48.0, 44.0));
    painter->drawLine(QPointF(62.0, 44.0), QPointF(66.0, 44.0));

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255, 255, 255, 240));
    painter->drawEllipse(QPointF(45.0, 44.0), 5.5, 7.0);
    painter->drawEllipse(QPointF(65.0, 44.0), 5.5, 7.0);
    painter->setBrush(QColor(55, 30, 20, 230));
    painter->drawEllipse(QPointF(46.0, 45.0), 2.2, 3.6);
    painter->drawEllipse(QPointF(64.0, 45.0), 2.2, 3.6);

    painter->setBrush(QColor(255, 154, 170, 225));
    painter->drawEllipse(QPointF(55.0, 54.0), 4.2, 2.8);

    painter->setPen(QPen(QColor(92, 46, 20, 220), 1.8, Qt::SolidLine, Qt::RoundCap));
    painter->drawArc(QRectF(48.0, 55.0, 14.0, 10.0), 195 * 16, 150 * 16);
    painter->drawLine(QPointF(39.0, 57.0), QPointF(18.0, 54.0));
    painter->drawLine(QPointF(39.0, 61.0), QPointF(16.0, 63.0));
    painter->drawLine(QPointF(71.0, 57.0), QPointF(92.0, 54.0));
    painter->drawLine(QPointF(71.0, 61.0), QPointF(94.0, 63.0));

    painter->setPen(QPen(QColor(120, 68, 28, 130), 4.0, Qt::SolidLine, Qt::RoundCap));
    painter->drawLine(QPointF(44.0, 29.0), QPointF(44.0, 16.0));
    painter->drawLine(QPointF(55.0, 27.0), QPointF(55.0, 13.0));
    painter->drawLine(QPointF(66.0, 29.0), QPointF(66.0, 16.0));

    painter->restore();
}

void RenderWidget::DrawLights(QPainter* painter) {
    painter->save();
    painter->setPen(Qt::NoPen);

    const double scene_scale = std::max(width(), height());
    if (controller_.AreSoftShadowsEnabled()) {
        painter->setCompositionMode(QPainter::CompositionMode_Screen);
        const double dynamic_radius = 0.85 * scene_scale;
        for (const QPointF& source : controller_.GetDynamicLightSources()) {
            DrawLightArea(
                painter, source, QColor(255, 250, 222, 196), QColor(255, 186, 92, 0),
                dynamic_radius);
        }
    } else {
        const auto dynamic_sources = controller_.GetDynamicLightSources();
        if (!dynamic_sources.empty()) {
            const Polygon light_area =
                controller_.CreateLightAreaForSource(dynamic_sources.front());
            const QPolygonF polygon = ToQPolygonF(light_area);
            if (polygon.size() >= 3) {
                QPainterPath light_path;
                light_path.addPolygon(polygon);
                light_path.closeSubpath();

                QRadialGradient gradient(dynamic_sources.front(), 0.78 * scene_scale);
                gradient.setColorAt(0.0, QColor(255, 250, 228, 220));
                gradient.setColorAt(0.28, QColor(255, 226, 132, 136));
                gradient.setColorAt(1.0, QColor(255, 184, 72, 24));

                painter->fillPath(light_path, gradient);
                painter->setPen(QPen(QColor(255, 236, 170, 115), 1.2));
                painter->drawPolygon(polygon);
                painter->setPen(Qt::NoPen);
            }
        }
    }

    painter->setCompositionMode(QPainter::CompositionMode_Screen);
    const double static_radius = 0.55 * scene_scale;
    for (const QPointF& source : controller_.GetStaticLights()) {
        DrawLightArea(
            painter, source, QColor(204, 240, 255, 140), QColor(104, 178, 255, 0), static_radius);
    }

    painter->restore();
}

void RenderWidget::DrawObstacles(QPainter* painter) {
    painter->save();

    QPen border_pen(cat_overlay_enabled_ ? QColor(255, 224, 196) : QColor(206, 208, 230));
    border_pen.setWidthF(1.5);
    painter->setPen(border_pen);
    painter->setBrush(cat_overlay_enabled_ ? QColor(28, 24, 30, 228) : QColor(32, 34, 46, 235));

    for (const Polygon& polygon : controller_.GetPolygons()) {
        const QPolygonF qpolygon = ToQPolygonF(polygon);
        if (qpolygon.size() >= 3) {
            painter->drawPolygon(qpolygon);
        } else if (qpolygon.size() == 2) {
            painter->drawPolyline(qpolygon);
        }

        painter->setPen(QPen(QColor(255, 148, 132), 4.0));
        for (const QPointF& vertex : polygon.GetVertices()) {
            painter->drawPoint(vertex);
        }
        painter->setPen(border_pen);
    }

    painter->restore();
}

void RenderWidget::DrawLightMarkers(QPainter* painter) {
    painter->save();

    painter->setPen(Qt::NoPen);
    if (mode_ == InteractionMode::kLight) {
        const auto dynamic_sources = controller_.GetDynamicLightSources();
        for (std::size_t i = 0; i < dynamic_sources.size(); ++i) {
            const bool primary = i == 0U;
            painter->setBrush(primary ? QColor(255, 246, 196) : QColor(255, 232, 164, 220));
            painter->drawEllipse(dynamic_sources[i], primary ? 4.5 : 3.5, primary ? 4.5 : 3.5);
        }

        if (!dynamic_sources.empty()) {
            painter->setPen(QPen(QColor(255, 244, 204, 210), 1.4));
            painter->setBrush(
                controller_.AreSoftShadowsEnabled() ? QColor(255, 220, 130, 38)
                                                    : QColor(255, 214, 112, 56));
            const double halo = controller_.AreSoftShadowsEnabled() ? 10.0 : 11.5;
            painter->drawEllipse(dynamic_sources.front(), halo, halo);
            painter->setPen(Qt::NoPen);
            painter->setBrush(QColor(255, 250, 220));
            painter->drawEllipse(dynamic_sources.front(), 2.5, 2.5);
        }
    }

    painter->setBrush(QColor(150, 228, 255));
    for (const QPointF& source : controller_.GetStaticLights()) {
        painter->drawEllipse(source, 3.5, 3.5);
    }

    painter->restore();
}

void RenderWidget::UpdateLightPosition(const QPointF& position) {
    const QPointF before = controller_.GetLightSource();
    controller_.SetLightSource(position);
    if (controller_.GetLightSource() == before && position != before) {
        ShowStatus("Источник света нельзя завести внутрь препятствия.");
    }
    update();
}

void RenderWidget::ShowStatus(const QString& text) const {
    if (status_callback_) {
        status_callback_(text);
    }
}

}  // namespace raycaster
