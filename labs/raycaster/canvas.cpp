#include "canvas.h"

#include <QDebug>
#include <QDirIterator>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <algorithm>

Canvas::Canvas(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true);

    if (!imgLight.load("labs/raycaster/light.jpg")) {
        qDebug() << "CRITICAL: light.jpg NOT FOUND.";
    }

    if (!imgShadow.load("labs/raycaster/shadow.jpg")) {
        qDebug() << "CRITICAL: shadow.jpg NOT FOUND";
    }
}

void Canvas::resizeEvent(QResizeEvent* event) {
    controller.UpdateBounds(event->size().width(), event->size().height());
    QWidget::resizeEvent(event);
}

void Canvas::SetMode(Mode mode) {
    if (mode == Mode::Light) {
        static_lights.clear();
    }
    currentMode = mode;
    isDrawing = false;
    update();
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    if (currentMode == Mode::Polygons) {
        if (event->button() == Qt::LeftButton) {
            if (!isDrawing) {
                controller.AddPolygon(Polygon({event->position(), event->position()}));
                isDrawing = true;
            } else {
                controller.AddVertexToLastPolygon(event->position());
            }
        } else if (event->button() == Qt::RightButton) {
            isDrawing = false;
        }
    } else if (currentMode == Mode::StaticLights && event->button() == Qt::LeftButton) {
        if (!controller.CheckCollision(event->position(), 8.0)) {
            static_lights.push_back({event->position(), QColor(200, 255, 200)});
        }
    }
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    if (currentMode == Mode::Light) {
        QPointF new_pos = event->position();

        double margin = 11.0;
        new_pos.setX(std::clamp(new_pos.x(), margin, (double)width() - margin));
        new_pos.setY(std::clamp(new_pos.y(), margin, (double)height() - margin));

        if (!controller.CheckCollision(new_pos, 8.0)) {
            controller.SetLightSource(new_pos);
        }
    } else if (currentMode == Mode::Polygons && isDrawing) {
        controller.UpdateLastPolygon(event->position());
    }
    update();
}

void Canvas::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (photoEnabled) {
        painter.drawPixmap(rect(), imgShadow);
    } else {
        painter.fillRect(rect(), Qt::black);
    }

    if (lightEnabled) {
        QPainterPath totalHardLightPath;
        totalHardLightPath.setFillRule(Qt::WindingFill);

        std::vector<std::pair<QPointF, QColor>> sources;

        if (currentMode != Mode::StaticLights) {
            sources.push_back({controller.GetLightSource(), QColor(255, 255, 220)});
        }

        for (const auto& sl : static_lights) {
            sources.push_back({sl.pos, sl.color});
        }

        for (const auto& src : sources) {
            const QPointF center_pos = src.first;

            if (!photoEnabled && shadowDetail > 0) {
                int soft_samples = shadowDetail;
                double soft_radius = 10.0;
                QColor areaColor = src.second;
                areaColor.setAlpha(255 / (soft_samples + 1));

                painter.setBrush(areaColor);
                painter.setPen(Qt::NoPen);

                for (int i = 0; i < soft_samples; ++i) {
                    double angle = (2.0 * M_PI * i) / soft_samples;
                    QPointF offset(std::cos(angle) * soft_radius, std::sin(angle) * soft_radius);

                    controller.SetLightSource(center_pos + offset);
                    Polygon lightArea = controller.CreateLightArea();

                    QPolygonF qp;
                    for (const auto& v : lightArea.GetVertices()) {
                        qp << v;
                    }
                    painter.drawPolygon(qp);
                }
            }

            controller.SetLightSource(center_pos);
            Polygon hardArea = controller.CreateLightArea();
            QPolygonF hardQp;
            for (const auto& v : hardArea.GetVertices()) {
                hardQp << v;
            }

            if (!photoEnabled) {
                QColor hardColor = src.second;
                hardColor.setAlpha(shadowDetail == 0 ? 255 : 100);
                painter.setBrush(hardColor);
                painter.setPen(Qt::NoPen);
                painter.drawPolygon(hardQp);
            }

            totalHardLightPath.addPolygon(hardQp);
        }

        if (currentMode != Mode::StaticLights && !sources.empty()) {
            controller.SetLightSource(sources[0].first);
        }

        if (photoEnabled && !totalHardLightPath.isEmpty()) {
            painter.setClipPath(totalHardLightPath);
            painter.drawPixmap(rect(), imgLight);
            painter.setClipping(false);
        }

        for (const auto& src : sources) {
            drawLightTexture(painter, src.first, src.second);
        }
    }

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::white, 2));

    const auto& polys = controller.GetPolygons();
    for (size_t i = 1; i < polys.size(); ++i) {
        QPolygonF qp;
        for (const auto& v : polys[i].GetVertices()) {
            qp << v;
        }
        painter.drawPolygon(qp);
    }
}

void Canvas::drawLightTexture(QPainter& painter, const QPointF& pos, const QColor& color) {
    painter.setPen(Qt::NoPen);
    QColor glow = color;
    glow.setAlpha(100);
    painter.setBrush(glow);
    painter.drawEllipse(pos, 8, 8);
    painter.setBrush(Qt::white);
    painter.drawEllipse(pos, 4, 4);
}