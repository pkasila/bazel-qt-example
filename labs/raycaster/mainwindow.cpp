#include "mainwindow.h"

#include <QMouseEvent>
#include <QPainter>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {

    resize(1000, 800);

    controller_.SetLightSource(QPointF(400, 300));

    Polygon border({
        QPointF(0, 0),
        QPointF(width(), 0),
        QPointF(width(), height()),
        QPointF(0, height())
    });

    controller_.AddPolygon(border);


}

void MainWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    painter.fillRect(rect(), QColor(20, 20, 20));

    Polygon light = controller_.CreateLightArea();

    QPolygonF light_poly;

    for (const auto& v : light.GetVertices()) {
        light_poly << v;
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 120, 180));
    if (light_poly.size() >= 3) {
        painter.drawPolygon(light_poly);
    }

    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::NoBrush);

    const auto& polygons = controller_.GetPolygons();

    for (size_t i = 1; i < polygons.size(); ++i) {
        QPolygonF poly;

        for (const auto& v : polygons[i].GetVertices()) {
            poly << v;
        }

        painter.drawPolygon(poly);
    }

    painter.setBrush(Qt::red);
    painter.drawEllipse(controller_.GetLightSource(), 5, 5);
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    controller_.SetLightSource(event->position());

    if (drawing_polygon_) {
        controller_.UpdateLastPolygon(event->position());
    }

    update();
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    QPointF pos = event->position();

    if (event->button() == Qt::LeftButton) {
        if (!drawing_polygon_) {
            Polygon polygon;
            polygon.AddVertex(pos);
            polygon.AddVertex(pos);

            controller_.AddPolygon(polygon);

            drawing_polygon_ = true;
        } else {
            controller_.AddVertexToLastPolygon(pos);
        }
    }

    if (event->button() == Qt::RightButton) {
        drawing_polygon_ = false;
    }

    update();
}
