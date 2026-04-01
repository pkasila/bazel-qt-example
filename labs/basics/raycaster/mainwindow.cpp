#include "mainwindow.h"
#include <QtWidgets/QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent), drawingPolygon_(false), currentMode_(LIGHT) {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    modeSelector_ = new QComboBox(this);
    modeSelector_->addItem("Light");
    modeSelector_->addItem("Polygons");
    QObject::connect(modeSelector_, 
                     static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), 
                     [this](int index) { modeChanged(index); });

    drawArea_ = new QWidget(this);
    drawArea_->setMinimumSize(800, 600);

    layout->addWidget(modeSelector_);
    layout->addWidget(drawArea_);
    setCentralWidget(centralWidget);

    std::vector<QPointF> boundary = {{0, 0}, {800, 0}, {800, 600}, {0, 600}};
    controller_.AddPolygon(Polygon(boundary));
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (const QPointF& source : controller_.getLightSources()) {
        Polygon lightArea = controller_.CreateLightArea(source);
        std::vector<QPointF> lightVertices = lightArea.getVertices();
        if (!lightVertices.empty()) {
            std::vector<QPoint> points;
            for (const QPointF& vertex : lightVertices) {
                points.push_back(vertex.toPoint());
            }
            painter.setBrush(QColor(255, 255, 200, 50));
            painter.setPen(Qt::NoPen);
            painter.drawPolygon(points.data(), points.size());
        }
    }

    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    for (const Polygon& poly : controller_.GetPolygons()) {
        for (size_t i = 0; i < poly.getVertices().size(); ++i) {
            painter.drawLine(poly.getVertices()[i], 
                             poly.getVertices()[(i + 1) % poly.getVertices().size()]);
        }
    }

    if (currentMode_ == LIGHT) {
        for (const QPointF& source : controller_.getLightSources()) {
            painter.setBrush(Qt::yellow);
            painter.drawEllipse(source, 3, 3);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (currentMode_ == POLYGONS) {
        if (event->button() == Qt::LeftButton) {
            if (!drawingPolygon_) {
                controller_.AddPolygon(Polygon({event->pos()}));
                drawingPolygon_ = true;
            } else {
                controller_.AddVertexToLastPolygon(event->pos());
            }
        } else if (event->button() == Qt::RightButton && drawingPolygon_) {
            drawingPolygon_ = false;
        }
    }
    update();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (currentMode_ == LIGHT) {
        controller_.setLightSource(event->pos());
        update();
    } else if (drawingPolygon_) {
        controller_.UpdateLastPolygon(event->pos());
        update();
    }
}

void MainWindow::modeChanged(int index) {
    currentMode_ = static_cast<Mode>(index);
    drawingPolygon_ = false;
}