#include "mainwindow.h"
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setMouseTracking(true);
    ctrl.AddPolygon(Polygon({{0,0}, {1000,0}, {1000,1000}, {0,1000}}));
}

void MainWindow::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), Qt::black);
    QPointF origin = ctrl.getLightSource();
    for (int i = -2; i <= 2; ++i) {
        ctrl.setLightSource(origin + QPointF(i*2, 0));
        Polygon area = ctrl.CreateLightArea();
        p.setBrush(QColor(255, 255, 100, 40));
        p.setPen(Qt::NoPen);
        p.drawPolygon(area.getVertices().data(), area.getVertices().size());
    }
    ctrl.setLightSource(origin);
    p.setPen(QPen(Qt::white, 2));
    for (const auto& poly : ctrl.GetPolygons()) p.drawPolygon(poly.getVertices().data(), poly.getVertices().size());
}

void MainWindow::mousePressEvent(QMouseEvent *e) {
    if (mode == Polygons) {
        if (e->button() == Qt::LeftButton) {
            if (!is_drawing) { ctrl.AddPolygon(Polygon()); ctrl.AddVertexToLastPolygon(e->pos()); is_drawing = true; }
            ctrl.AddVertexToLastPolygon(e->pos());
        } else is_drawing = false;
    }
    update();
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) {
    if (mode == Light) ctrl.setLightSource(e->pos());
    else if (is_drawing) ctrl.UpdateLastPolygon(e->pos());
    update();
}

void MainWindow::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_L) mode = Light;
    if (e->key() == Qt::Key_P) mode = Polygons;
    update();
}