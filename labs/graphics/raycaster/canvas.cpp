#include "canvas.h"
#include <QPainter>
#include <QMouseEvent>

Canvas::Canvas(QWidget *parent) : QWidget(parent) {


    std::vector<QPointF> vertices = {
        QPointF(0, 0),
        QPointF(width(), 0),
        QPointF(width(), height()),
        QPointF(0, height())
    };
    m_controller.addPolygon(Polygon(vertices));
}

void Canvas::setPolygonMode(bool enabled) {
    if (enabled) {
        m_mode = PolygonMode;
        m_controller.addPolygon(Polygon());
    }
}
void Canvas::setLightMode(bool enabled) { if (enabled) m_mode = LightMode; }

void Canvas::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawBoundingPolygon(painter);
    drawUserPolygons(painter);
    if (m_mode == LightMode){
        drawLight(painter);
    }

}

void Canvas::mousePressEvent(QMouseEvent *event) {
    QPointF pos = event->pos();
    if (m_mode == LightMode) {
        if (m_controller.setLightSource(pos)) {
            update();
        }
    }
    if (m_mode == PolygonMode) {
        if (event->button() == Qt::LeftButton){
             m_controller.addVertexToLastPolygon(pos);
        }
        else if (event->button() == Qt::RightButton) m_controller.addPolygon(Polygon());
    } else m_controller.setLightSource(pos);
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
    QPointF pos = event->pos();
    if (m_mode == LightMode) {
        if (m_controller.setLightSource(pos)) {
            update();
        }
    }
    if (m_mode == PolygonMode) {
        m_controller.updateLastPolygonVertex(event->pos());
        update();
    }
}

void Canvas::resizeEvent(QResizeEvent *event) {
    updateBoundingArea();
    QWidget::resizeEvent(event);
}

void Canvas::updateBoundingArea() {
    const int margin = 20;
    m_controller.updateBoundingPolygon(QPointF(margin, margin), QPointF(width() - margin, height() - margin));
}

void Canvas::drawBoundingPolygon(QPainter &painter) {
    painter.setBrush(QColor(245, 245, 245));
    painter.setPen(Qt::NoPen);
    const auto &vertices = m_controller.getPolygons()[0].getVertices();
    painter.drawPolygon(vertices.data(), vertices.size());
}

void Canvas::drawUserPolygons(QPainter &painter) {
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(Qt::NoBrush);
    for (size_t i = 1; i < m_controller.getPolygons().size(); ++i) {
        const auto &vertices = m_controller.getPolygons()[i].getVertices();
        if (vertices.size() > 1) {
            painter.drawPolyline(vertices.data(), vertices.size());
            if (vertices.size() >= 3) painter.drawLine(vertices.back(), vertices.front());
        }
    }
}

void Canvas::drawLight(QPainter &painter) {
    painter.setPen(Qt::red);
    painter.drawEllipse(m_controller.getLightSource(), 5, 5);
    Polygon lightArea = m_controller.createLightArea();
    painter.setBrush(QColor(255, 255, 0, 60));
    painter.drawPolygon(lightArea.getVertices().data(), lightArea.getVertices().size());
}


