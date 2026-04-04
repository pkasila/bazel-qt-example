#include "main_window.h"
#include <QApplication>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPolygon>

DrawingWidget::DrawingWidget(QWidget* parent) 
    : QWidget(parent), controller_(nullptr), is_drawing_polygon_(false) {
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

void DrawingWidget::mousePressEvent(QMouseEvent* event) {
    if (!controller_) return;
    
    QPointF pos = event->pos();
    
    if (mode_ == "polygons") {
        if (event->button() == Qt::LeftButton) {
            if (!is_drawing_polygon_) {
                controller_->AddPolygon(Polygon());
                controller_->AddVertexToLastPolygon(pos);
                is_drawing_polygon_ = true;
            } else {
                controller_->AddVertexToLastPolygon(pos);
            }
        } else if (event->button() == Qt::RightButton) {
            is_drawing_polygon_ = false;
        }
    } else if (mode_ == "light") {
        controller_->setLightSource(pos);
    }
    
    update();
}

void DrawingWidget::mouseMoveEvent(QMouseEvent* event) {
    if (!controller_) return;
    
    QPointF pos = event->pos();
    
    if (mode_ == "polygons" && is_drawing_polygon_) {
        controller_->UpdateLastPolygon(pos);
    } else if (mode_ == "light") {
        controller_->setLightSource(pos);
    }
    
    update();
}

void DrawingWidget::keyPressEvent(QKeyEvent* event) {
    if (!controller_ || mode_ != "light") return;
    
    QPointF current_pos = controller_->getLightSource();
    double step = 5.0;
    
    switch (event->key()) {
        case Qt::Key_Up:
            current_pos.setY(current_pos.y() - step);
            break;
        case Qt::Key_Down:
            current_pos.setY(current_pos.y() + step);
            break;
        case Qt::Key_Left:
            current_pos.setX(current_pos.x() - step);
            break;
        case Qt::Key_Right:
            current_pos.setX(current_pos.x() + step);
            break;
        default:
            QWidget::keyPressEvent(event);
            return;
    }
    
    controller_->setLightSource(current_pos);
    update();
}

void DrawingWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Очистка фона
    painter.fillRect(rect(), Qt::black);
    
    if (!controller_) return;
    
    if (mode_ == "polygons") {
        drawPolygons(painter);
    } else if (mode_ == "light") {
        drawLightAndShadows(painter);
        drawLightAreas(painter);
    }
    
    drawLightSource(painter);
}

void DrawingWidget::drawLightSource(QPainter& painter) {
    if (mode_ == "light") {
        // Рисуем все источники света
        const auto& light_sources = controller_->GetLightSources();
        for (const auto& light : light_sources) {
            painter.setPen(QPen(Qt::yellow, 3));
            painter.drawPoint(light);
            painter.setPen(QPen(Qt::yellow, 1));
            painter.drawEllipse(light, 5, 5);
        }
        
        // Если источников еще нет, рисуем основной
        if (light_sources.empty()) {
            QPointF light = controller_->getLightSource();
            painter.setPen(QPen(Qt::yellow, 3));
            painter.drawPoint(light);
            painter.setPen(QPen(Qt::yellow, 1));
            painter.drawEllipse(light, 5, 5);
        }
    } else {
        // В режиме многоугольников рисуем один источник света
        QPointF light = controller_->getLightSource();
        painter.setPen(QPen(Qt::yellow, 2));
        painter.drawPoint(light);
    }
}

void DrawingWidget::drawPolygons(QPainter& painter) {
    const auto& polygons = controller_->GetPolygons();
    
    for (const auto& polygon : polygons) {
        const auto& vertices = polygon.getVertices();
        
        if (vertices.empty()) continue;
        
        // Рисуем многоугольник
        painter.setPen(QPen(Qt::white, 2));
        painter.setBrush(QBrush(Qt::darkGray));
        
        if (vertices.size() > 2) {
            QPolygon qpoly;
            for (const auto& vertex : vertices) {
                qpoly << vertex.toPoint();
            }
            painter.drawPolygon(qpoly);
        } else if (vertices.size() == 2) {
            painter.drawLine(vertices[0], vertices[1]);
        } else if (vertices.size() == 1) {
            painter.drawPoint(vertices[0]);
        }
        
        // Подсвечиваем вершины
        painter.setPen(QPen(Qt::red, 3));
        for (const auto& vertex : vertices) {
            painter.drawPoint(vertex);
        }
    }
}

void DrawingWidget::drawLightAndShadows(QPainter& painter) {
    // Создаем освещенную область
    Polygon light_area = controller_->CreateLightArea();
    const auto& vertices = light_area.getVertices();
    
    if (vertices.size() > 2) {
        QPolygon qpoly;
        for (const auto& vertex : vertices) {
            qpoly << vertex.toPoint();
        }
        
        // Рисуем освещенную область
        painter.setPen(QPen(Qt::yellow, 1));
        painter.setBrush(QBrush(QColor(255, 255, 0, 50)));
        painter.drawPolygon(qpoly);
    }
}

void DrawingWidget::drawLightAreas(QPainter& painter) {
    // Рисуем освещенные области для множественных источников
    auto light_areas = controller_->CreateLightAreas();
    
    for (const auto& light_area : light_areas) {
        const auto& vertices = light_area.getVertices();
        
        if (vertices.size() > 2) {
            QPolygon qpoly;
            for (const auto& vertex : vertices) {
                qpoly << vertex.toPoint();
            }
            
            // Рисуем полупрозрачную освещенную область
            painter.setPen(QPen(Qt::yellow, 1));
            painter.setBrush(QBrush(QColor(255, 255, 0, 30)));
            painter.drawPolygon(qpoly);
        }
    }
}

MainWindow::MainWindow(QWidget* parent) 
    : QMainWindow(parent) {
    controller_ = new Controller();
    setupUI();
    
    // Создаем граничный многоугольник
    controller_->CreateBoundaryPolygon(800, 600);
    
    setWindowTitle("RayCaster Lab");
    resize(800, 650);
}

void MainWindow::setupUI() {
    QWidget* central_widget = new QWidget();
    setCentralWidget(central_widget);
    
    QVBoxLayout* main_layout = new QVBoxLayout(central_widget);
    
    // Панель управления
    QWidget* control_panel = new QWidget();
    QHBoxLayout* control_layout = new QHBoxLayout(control_panel);
    
    mode_combo_ = new QComboBox();
    mode_combo_->addItem("polygons", "polygons");
    mode_combo_->addItem("light", "light");
    
    clear_button_ = new QPushButton("Clear All");
    add_light_button_ = new QPushButton("Add Light Source");
    
    status_label_ = new QLabel("Mode: polygons");
    
    control_layout->addWidget(new QLabel("Mode:"));
    control_layout->addWidget(mode_combo_);
    control_layout->addWidget(add_light_button_);
    control_layout->addWidget(clear_button_);
    control_layout->addStretch();
    control_layout->addWidget(status_label_);
    
    // Область рисования
    drawing_widget_ = new DrawingWidget();
    drawing_widget_->setController(controller_);
    drawing_widget_->setMode("polygons");
    drawing_widget_->setFixedSize(800, 600);
    drawing_widget_->setStyleSheet("background-color: black;");
    
    main_layout->addWidget(control_panel);
    main_layout->addWidget(drawing_widget_);
    
    // Подключаем сигналы
    connect(mode_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) {
                QString mode = mode_combo_->currentData().toString();
                drawing_widget_->setMode(mode);
                status_label_->setText("Mode: " + mode);
                drawing_widget_->update();
            });
    
    connect(clear_button_, &QPushButton::clicked, [this]() {
        controller_ = new Controller();
        controller_->CreateBoundaryPolygon(800, 600);
        drawing_widget_->setController(controller_);
        drawing_widget_->update();
    });
    
    connect(add_light_button_, &QPushButton::clicked, [this]() {
        if (mode_combo_->currentData().toString() == "light") {
            QPointF current_light = controller_->getLightSource();
            controller_->AddLightSource(current_light);
            status_label_->setText("Added light source. Total: " + 
                                QString::number(controller_->GetLightSources().size()));
        }
    });
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
