#include "canvaswidget.h"

#include <QMouseEvent>

CanvasWidget::CanvasWidget(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void CanvasWidget::setMode(InteractionMode mode) {
    current_mode = mode;
    update();
}

void CanvasWidget::setController(Controller* cntrl) {
    controller = cntrl;

    std::vector<QPointF> bigRectangle = {{0, 0}, {875, 0}, {875, 687.5}, {0, 687.5}};
    controller->AddPolygon(bigRectangle);
}

void CanvasWidget::mousePressEvent(QMouseEvent* event) {
    if (!controller) {
        return;
    }

    const QPointF clickPos = event->pos();

    if (controller->GetPolygons().empty()) {
        return;
    }

    const auto& borderPolygon = controller->GetPolygons()[0].GetVertices();

    if (!controller->IsPointInsidePolygon(clickPos, borderPolygon)) {
        return;
    }

    if (current_mode == InteractionMode::Polygons) {
        if (event->button() == Qt::LeftButton) {
            if (!drawingPolygon) {
                controller->AddPolygon(Polygon({event->pos()}));
                drawingPolygon = true;
            } else {
                controller->AddVertexToLastPolygon(event->pos());
            }
        }
        if (event->button() == Qt::RightButton && drawingPolygon) {
            drawingPolygon = false;
        }
    }
    update();
}

void CanvasWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawPolygons(painter);

    if (current_mode == InteractionMode::Light) {
        drawLightSource(painter);
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent* event) {
    const auto& borderPolygon = controller->GetPolygons()[0].GetVertices();

    if (!controller->IsPointInsidePolygon(event->pos(), borderPolygon)) {
        return;
    }
    if (!controller || current_mode != InteractionMode::Light) {
        return;
    }

    const auto& polygons = controller->GetPolygons();
    if (polygons.empty()) {
        return;
    }

    const auto& boundaryPolygon = polygons[0].GetVertices();

    QPointF cursorPos = event->pos();

    if (controller->IsPointInsidePolygon(cursorPos, boundaryPolygon)) {
        controller->SetLightSource(cursorPos);
        controller->GenerateShades();
        update();
    }
}

void CanvasWidget::drawPolygons(QPainter& painter) {
    if (!controller) {
        return;
    }

    painter.setPen(QPen(Qt::black));

    for (const Polygon& poly : controller->GetPolygons()) {
        std::vector<QPointF> vertices = poly.GetVertices();
        for (size_t i = 0; i < std::size(vertices); i++) {
            const QPointF& p1 = vertices[i];
            const QPointF& p2 = vertices[(i + 1) % vertices.size()];
            if (p1 != p2) {
                painter.drawLine(p1, p2);
            }
        }
    }
}

void CanvasWidget::drawLightSource(QPainter& painter) {
    if (!controller) {
        return;
    }

    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(controller->GetLightSource(), 5, 5);

    std::vector<Polygon> lightAreas = controller->CreateLightArea();
    for (size_t i = 0; i < lightAreas.size(); i++) {
        std::vector<QPointF> vertices = lightAreas[i].GetVertices();

        painter.setBrush(Qt::red);
        painter.setPen(Qt::NoPen);
        if (vertices.size() >= 3) {
            painter.setBrush(QColor(171, 36, 36, 100));

            painter.setPen(Qt::NoPen);
            painter.drawPolygon(vertices.data(), vertices.size());
        }
    }
}
