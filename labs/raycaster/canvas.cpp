#include "canvas.h"

#include <QPainter>
#include <QPainterPath>

CanvasWidget::CanvasWidget(QWidget* parent)
    : QWidget(parent), activeMode(RenderMode::Light), isDrawing(false), previewPt(0, 0) {
    setMouseTracking(true);
}

void CanvasWidget::setRenderMode(RenderMode newMode) {
    if (activeMode != newMode) {
        if (activeMode == RenderMode::Polygons && isDrawing) {
            controller.completePolygon();
            isDrawing = false;
        }
        activeMode = newMode;
        update();
    }
}

void CanvasWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    painter.fillRect(rect(), GlobalColors::BG_COLOR);
    painter.setRenderHint(QPainter::Antialiasing);
    double scaleX = static_cast<double>(width()) / 800.0;
    double scaleY = static_cast<double>(height()) / 600.0;
    painter.scale(scaleX, scaleY);
    painter.setPen(GlobalColors::STROKE_COLOR);
    for (const auto& poly : controller.getPolygons()) {
        auto closedVerts = poly.closedVertices();
        if (!closedVerts.empty()) {
            QPainterPath polyPath;
            polyPath.moveTo(poly.getVertices().front());
            for (size_t i = 1; i < poly.getVertices().size(); ++i) {
                polyPath.lineTo(poly.getVertices()[i]);
            }
            polyPath.closeSubpath();
            painter.fillPath(polyPath, QBrush(GlobalColors::FINISHED_FILL));
            for (size_t i = 0; i < closedVerts.size() - 1; ++i) {
                painter.drawLine(closedVerts[i], closedVerts[i + 1]);
            }
        }
    }
    if (activeMode == RenderMode::Light) {
        painter.setBrush(GlobalColors::LIGHT_COLOR);
        painter.setPen(Qt::NoPen);
        QPoint lightPos = controller.getLightPosition();
        painter.drawEllipse(
            lightPos, GlobalConfig::LIGHT_DIAMETER / 2, GlobalConfig::LIGHT_DIAMETER / 2);
        auto lightArea = controller.computeLightArea();
        if (!lightArea.empty()) {
            QPainterPath areaPath;
            areaPath.moveTo(lightArea.front());
            for (size_t i = 1; i < lightArea.size(); ++i) {
                areaPath.lineTo(lightArea[i]);
            }
            areaPath.closeSubpath();
            painter.fillPath(areaPath, QBrush(GlobalColors::LIGHT_AREA_FILL));
        }
    } else if (activeMode == RenderMode::Polygons) {
        const auto& polys = controller.getPolygons();
        if (!polys.empty()) {
            const auto& verts = polys.back().getVertices();
            if (!verts.empty()) {
                if (verts.size() >= 3) {
                    auto closedVerts = polys.back().closedVertices();
                    QPainterPath polyPath;
                    polyPath.moveTo(closedVerts.front());
                    for (size_t i = 1; i < closedVerts.size(); ++i) {
                        polyPath.lineTo(closedVerts[i]);
                    }
                    polyPath.closeSubpath();
                    painter.fillPath(polyPath, QBrush(GlobalColors::ACTIVE_FILL));
                    for (size_t i = 0; i < closedVerts.size() - 1; ++i) {
                        painter.drawLine(closedVerts[i], closedVerts[i + 1]);
                    }
                } else {
                    for (size_t i = 0; i < verts.size() - 1; ++i) {
                        painter.drawLine(verts[i], verts[i + 1]);
                    }
                    painter.drawLine(verts.back(), previewPt);
                }
            }
        }
    }
}

void CanvasWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    update();
}

QPoint CanvasWidget::convertToScene(const QPoint& widgetPos) const {
    double scaleX = static_cast<double>(width()) / 800.0;
    double scaleY = static_cast<double>(height()) / 600.0;
    return QPoint(
        static_cast<int>(widgetPos.x() / scaleX), static_cast<int>(widgetPos.y() / scaleY));
}

void CanvasWidget::mousePressEvent(QMouseEvent* event) {
    QPoint scenePos = convertToScene(event->pos());
    if (activeMode == RenderMode::Light) {
        controller.setLightPosition(scenePos);
    } else if (activeMode == RenderMode::Polygons) {
        if (event->button() == Qt::LeftButton) {
            if (!isDrawing) {
                isDrawing = true;
                controller.beginPolygon(scenePos);
                previewPt = scenePos;
            } else {
                controller.appendVertex(scenePos);
            }
        } else if (event->button() == Qt::RightButton) {
            isDrawing = false;
            controller.completePolygon();
        }
    }
    update();
}

void CanvasWidget::mouseMoveEvent(QMouseEvent* event) {
    QPoint scenePos = convertToScene(event->pos());
    if (activeMode == RenderMode::Light) {
        controller.setLightPosition(scenePos);
    } else if (activeMode == RenderMode::Polygons && isDrawing) {
        controller.updateCurrentPolygon(scenePos);
        previewPt = scenePos;
    }
    update();
}
