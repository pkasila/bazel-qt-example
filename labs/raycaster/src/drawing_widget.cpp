//
// Created by blalex on 4/8/25.
//

#include "drawing_widget.h"

#include "polygon.h"

#include <QPainter>
#include <QPainterPath>
#include <qcolor.h>
#include <qmath.h>
#include <qtmetamacros.h>

DrawingWidget::DrawingWidget(Controller* controller, QWidget* parent)
    : QWidget(parent), controller_(controller) {
    setMouseTracking(true);
}

void DrawingWidget::paintEvent(QPaintEvent* event) {
    QElapsedTimer timer;
    timer.start();

    QPainter painter(this);
    painter.fillRect(rect(), QColor(70, 51, 94));

    painter.setRenderHint(QPainter::Antialiasing);

    for (size_t i = 1; i < std::size(controller_->GetPolygons()); ++i) {
        const auto& polygon = controller_->GetPolygons()[i].GetVertices();
        FillPolygonArea(painter, polygon, QBrush(QColor("#dd237a")));
        painter.setPen(Qt::black);
        for (size_t i = 0; i < std::size(polygon); i++) {
            painter.drawLine(polygon[i], polygon[(i + 1) % std::size(polygon)]);
        }
    }

    if (controller_->GetMode() == 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(QColor(90, 66, 120)));
        QPointF light = controller_->GetLightSource();

        const Polygon light_area = controller_->CreateLightArea();
        QColor semiTransparentColor("#29292a");
        semiTransparentColor.setAlpha(51);
        QBrush brush(semiTransparentColor);
        painter.setPen(Qt::black);
        FillPolygonArea(painter, light_area.GetVertices(), brush);

        constexpr double angle_step = 2 * M_PI / 8;

        for (int i = 0; i < 8; ++i) {
            double angle = i * angle_step;

            double x = light.x() + 15 * std::cos(angle);
            double y = light.y() + 15 * std::sin(angle);
            controller_->SetLightSource(QPointF(x, y));
            const Polygon light_area_8 = controller_->CreateLightArea();
            FillPolygonArea(painter, light_area_8.GetVertices(), brush);
        }

        painter.drawEllipse(light, 4, 4);

        for (int i = 0; i < 8; ++i) {
            const double angle = i * angle_step;

            const double x = light.x() + 15 * std::cos(angle);
            const double y = light.y() + 15 * std::sin(angle);
            painter.drawEllipse(QPointF(x, y), 4, 4);
        }
    }
    QString fpsText;
    int const frame_time = timer.nsecsElapsed() / 1000.0;
    if (int const fps = 1e6 / frame_time; fps > 10'000) {
        fpsText = "∞ FPS";
    } else {
        fpsText = QString::asprintf("%i FPS", fps);
    }

    emit FPSUpdated(fpsText);
}

void DrawingWidget::mouseMoveEvent(QMouseEvent* event) {
    if (controller_->GetMode() == 0) {
        controller_->SetLightSource(event->pos());
    } else if (drawing_polygon_) {
        controller_->UpdateLastPolygon(event->pos());
    }
    update();
}

void DrawingWidget::mousePressEvent(QMouseEvent* event) {
    if (controller_->GetMode() == 0) {
        controller_->SetLightSource(event->pos());
    } else {
        if (event->button() == Qt::LeftButton) {
            if (!drawing_polygon_) {
                drawing_polygon_ = true;
                Polygon new_poly;
                new_poly.AddVertex(event->pos());
                new_poly.AddVertex(event->pos());
                controller_->AddPolygon(new_poly);
            } else {
                controller_->AddVertexToLastPolygon(event->pos());
            }
        } else if (event->button() == Qt::RightButton) {
            drawing_polygon_ = false;
        }
    }
    update();
}

void DrawingWidget::FillPolygonArea(
    QPainter& painter, const std::vector<QPointF>& points, const QBrush& fill_color) {
    if (points.empty()) {
        return;
    }

    QPainterPath path;

    path.moveTo(points[0]);
    for (size_t i = 1; i < std::size(points); ++i) {
        path.lineTo(points[i]);
    }

    path.closeSubpath();
    painter.fillPath(path, fill_color);
}

void DrawingWidget::resizeEvent(QResizeEvent* event) {
    controller_->UpdateBorderRect(event->size());
    // QOpenGLWidget::resizeEvent(event);
    QWidget::resizeEvent(event);
}
