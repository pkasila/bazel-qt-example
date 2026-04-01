#include "mainwindow.h"

#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainterPath>
#include <QRadialGradient>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUI();
    updateScene();
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    scene_ = new QGraphicsScene(this);
    scene_->setBackgroundBrush(Qt::black);
    view_ = new QGraphicsView(scene_);
    view_->setRenderHint(QPainter::Antialiasing);
    view_->setMouseTracking(true);
    view_->viewport()->installEventFilter(this);
    view_->setSceneRect(-500, -500, 1000, 1000);
    mode_ = new QComboBox();
    mode_->addItem("Light Mode");
    mode_->addItem("Polygons Mode");
    QVBoxLayout* main_layout = new QVBoxLayout();
    QHBoxLayout* control_layout = new QHBoxLayout();
    control_layout->addWidget(new QLabel("Mode:"));
    control_layout->addWidget(mode_);
    control_layout->addStretch();
    main_layout->addLayout(control_layout);
    main_layout->addWidget(view_);
    QWidget* central_widget = new QWidget();
    central_widget->setLayout(main_layout);
    setCentralWidget(central_widget);
    connect(
        mode_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &MainWindow::onModeChanged);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    updateScene();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF scenePos = view_->mapToScene(mouseEvent->pos());
        cur_mouse_pos_ = scenePos;

        if (mode_->currentIndex() == 0) {
            QPointF main_light = scenePos;
            std::vector<QPointF> light_sources = {
              main_light, main_light + QPointF(10, 10), main_light + QPointF(-10, 10),
              main_light + QPointF(-10, -10), main_light + QPointF(10, -10)};
            controller_.SetLightSources(light_sources);
            updateScene();
        }
        return true;
    }
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF scenePos = view_->mapToScene(mouseEvent->pos());

        if (mouseEvent->button() == Qt::LeftButton) {
            onSceneClicked(scenePos);
            return true;
        } else if (mouseEvent->button() == Qt::RightButton && mode_->currentIndex() == 1) {
            is_drawing_poly_ = false;
            updateScene();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::onModeChanged(int index) {
    updateScene();
}

void MainWindow::onSceneClicked(const QPointF& point) {
    if (mode_->currentIndex() == 0) {
        QPointF main_light = point;
        std::vector<QPointF> light_sources = {
          main_light, main_light + QPointF(10, 10), main_light + QPointF(-10, 10),
          main_light + QPointF(-10, -10), main_light + QPointF(10, -10)};
        controller_.SetLightSources(light_sources);
    } else {
        if (!is_drawing_poly_) {
            Polygon new_poly({point});
            controller_.AddPolygon(new_poly);
            is_drawing_poly_ = true;
        } else {
            controller_.AddVertexToLastPolygon(point);
        }
    }
    updateScene();
}

void MainWindow::updateScene() {
    scene_->clear();
    drawPolygons();
    drawLightArea();
    drawLightSource();
}

void MainWindow::drawPolygons() {
    QPen pen(Qt::lightGray, 1.5);
    QBrush brush(QColor(100, 100, 100, 200));

    for (const auto& poly : controller_.GetPolygons()) {
        const auto& vers = poly.GetVertices();
        if (vers.size() < 2) {
            continue;
        }

        QPolygonF poly1;
        for (const auto& ver : vers) {
            poly1 << ver;
        }
        scene_->addPolygon(poly1, pen, brush);
        for (const auto& ver : vers) {
            scene_->addEllipse(
                ver.x() - 3, ver.y() - 3, 6, 6, QPen(Qt::darkGray), QBrush(Qt::darkGray));
        }
    }
}

void MainWindow::drawLightArea() {
    QRectF bound = view_->sceneRect();
    if (bound.isNull()) {
        bound = QRectF(-500, -500, 1000, 1000);
    }
    const auto& source = controller_.GetLightSources();
    if (source.empty()) {
        return;
    }
    for (const auto& light_pos : source) {
        Polygon light_area = controller_.CreateLightArea(light_pos, bound);
        const auto& vers = light_area.GetVertices();

        if (vers.size() >= 3) {
            QPainterPath path;
            path.moveTo(vers[0]);
            for (size_t i = 1; i < vers.size(); ++i) {
                path.lineTo(vers[i]);
            }
            path.closeSubpath();
            QRadialGradient gradient(light_pos, 600);
            gradient.setColorAt(0, QColor(255, 200, 150, 80));
            gradient.setColorAt(1, QColor(255, 150, 100, 0));
            scene_->addPath(path, QPen(Qt::NoPen), QBrush(gradient))->setZValue(5);
        }
    }
}

void MainWindow::drawLightSource() {
    const auto& light_sources = controller_.GetLightSources();
    const qreal glow_r = 30.0;
    const qreal core_r = 8.0;
    for (const auto& light_pos : light_sources) {
        QRadialGradient glow_grad(light_pos, glow_r);
        glow_grad.setColorAt(0, QColor(255, 220, 180, 100));
        glow_grad.setColorAt(1, QColor(255, 180, 120, 0));
        scene_
            ->addEllipse(
                light_pos.x() - glow_r, light_pos.y() - glow_r, glow_r * 2, glow_r * 2,
                QPen(Qt::NoPen), QBrush(glow_grad))
            ->setZValue(10);
        scene_
            ->addEllipse(
                light_pos.x() - core_r, light_pos.y() - core_r, core_r * 2, core_r * 2,
                QPen(Qt::NoPen), QBrush(QColor(255, 240, 200)))
            ->setZValue(11);
    }
}
