#include "mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QPolygonF>
#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setMouseTracking(true);
    showFullScreen();

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    lightBtn = new QRadioButton("Light Mode", this);
    polyBtn = new QRadioButton("Polygons Mode", this);
    polyBtn->setChecked(true);

    layout->addWidget(lightBtn);
    layout->addWidget(polyBtn);

    connect(lightBtn, &QRadioButton::toggled, [this](bool c){ 
        if(c) { mode = LIGHT; drawing = false; } 
    });
    connect(polyBtn, &QRadioButton::toggled, [this](bool c){ 
        if(c) { mode = POLYGONS; drawing = false; } 
    });

    QRect screen = QGuiApplication::primaryScreen()->geometry();
    controller.AddPolygon(Polygon({
        QPointF(0, 0), 
        QPointF((double)screen.width(), 0), 
        QPointF((double)screen.width(), (double)screen.height()), 
        QPointF(0, (double)screen.height())
    }));
}

MainWindow::~MainWindow() {}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) this->close();
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    if (mode == POLYGONS) {
        if (event->button() == Qt::LeftButton) {
            if (!drawing) {
                controller.AddPolygon(Polygon({event->position()}));
                drawing = true;
            } else {
                controller.AddVertexToLastPolygon(event->position());
            }
        } else if (event->button() == Qt::RightButton) {
            drawing = false;
        }
    }
    update();
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    if (mode == LIGHT) controller.setLightSource(event->position());
    update();
}

void MainWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::black);

    QPointF center = controller.getLightSource();
    std::vector<QPointF> sources = {
        center, 
        {center.x() + 4, center.y() + 4}, 
        {center.x() - 4, center.y() - 4},
        {center.x() + 4, center.y() - 4},
        {center.x() - 4, center.y() + 4}
    };

    if (mode == LIGHT) {
        for (const auto& src : sources) {
            Polygon lightArea = controller.CreateLightArea(src);
            QPolygonF qp; 
            for (const auto& v : lightArea.getVertices()) qp << v;
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(255, 255, 200, 35)); 
            painter.drawPolygon(qp);
        }
    }

    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::NoBrush);
    const auto& polys = controller.GetPolygons();
    
    for (size_t i = 0; i < polys.size(); ++i) {
        const auto& v = polys[i].getVertices();
        if (v.empty()) continue;

        bool current = (i == polys.size() - 1 && drawing);

        for (size_t j = 0; j < v.size(); ++j) {
            if (current) {
                if (j + 1 < v.size()) painter.drawLine(v[j], v[j+1]);
            } else {
                painter.drawLine(v[j], v[(j + 1) % v.size()]);
            }
        }
    }
}