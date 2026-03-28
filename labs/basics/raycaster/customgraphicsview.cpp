#include "customgraphicsview.h"
#include <QMouseEvent>
#include <QPainterPath>

CustomGraphicsView::CustomGraphicsView(QWidget *parent)
    : QGraphicsView(parent), isLightMode_(true) {
    setRenderHint(QPainter::Antialiasing);
    setMouseTracking(true);
    setScene(new QGraphicsScene(this));
    setSceneRect(0, 0, 800, 600);
}

void CustomGraphicsView::setPolygons(const std::vector<std::vector<QPointF>>& polygons) {
    polygons_ = polygons;
}

void CustomGraphicsView::setCurrentPolygon(const std::vector<QPointF>& polygon) {
    currentPolygon_ = polygon;
}

void CustomGraphicsView::setLight(const QPointF& pos) {
    lightPos_ = pos;
}

void CustomGraphicsView::setMode(bool isLightMode) {
    isLightMode_ = isLightMode;
}

void CustomGraphicsView::paintEvent(QPaintEvent *event) {
    QGraphicsView::paintEvent(event);
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    for (const auto& poly : polygons_) {
        drawPolygon(painter, poly);
    }
    if (!isLightMode_ && !currentPolygon_.empty()) {
        drawPolygon(painter, currentPolygon_, Qt::red, Qt::lightGray, true);
    }

    if (isLightMode_ && !lightArea.empty()) {
        drawPolygon(painter, lightArea, Qt::yellow, QColor(255, 255, 150, 100));
    }

    // Draw light source
    painter.setBrush(Qt::yellow);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(lightPos_, 5, 5);
}

void CustomGraphicsView::mousePressEvent(QMouseEvent *event) {
    emit sceneMousePressed(mapToScene(event->pos()), event->button());
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    emit sceneMouseMoved(mapToScene(event->pos()));
}

void CustomGraphicsView::drawPolygon(QPainter &painter, const std::vector<QPointF> &vertices,
                                     const QColor &borderColor, const QColor &fillColor, bool dashed) {
    if (vertices.empty()) return;

    painter.setPen(QPen(borderColor, 2, dashed ? Qt::DashLine : Qt::SolidLine));
    for (size_t i = 0; i < vertices.size(); ++i) {
        size_t next = (i + 1) % vertices.size();
        painter.drawLine(vertices[i], vertices[next]);
    }

    if (vertices.size() >= 3) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(fillColor);

        QPainterPath path;
        path.moveTo(vertices[0]);
        for (size_t i = 1; i < vertices.size(); ++i) {
            path.lineTo(vertices[i]);
        }
        path.closeSubpath();
        painter.drawPath(path);
    }
}
