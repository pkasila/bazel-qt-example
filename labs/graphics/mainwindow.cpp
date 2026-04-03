#include "mainwindow.h"

#include <QSize>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    plane_ = new Plane(this);
    setCentralWidget(plane_);
    setWindowTitle("RayCaster");
    setFixedSize(QSize(1080, 720));
    plane_->controller_.AddPolygon(Polygon());
    plane_->controller_.AddVertexToLastPolygon(QPoint(-1, -1));
    plane_->controller_.AddVertexToLastPolygon(QPoint(-1, height()));
    plane_->controller_.AddVertexToLastPolygon(QPoint(width(), height()));
    plane_->controller_.AddVertexToLastPolygon(QPoint(width(), -1));
}

MainWindow::~MainWindow() = default;
