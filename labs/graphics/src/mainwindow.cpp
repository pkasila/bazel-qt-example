#include "../input/controller.h"
#include "../input/mainwindow.h"
#include <QBrush>
#include <QColor>
#include <QComboBox>
#include <QDebug>
#include <QElapsedTimer>
#include <QFont>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPushButton>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <vector>

const int ObjectTypeRole = Qt::UserRole + 1;
const int ObjectIndexRole = Qt::UserRole + 2;

// ================== DrawingWidget Implementation ==================

DrawingWidget::DrawingWidget(Controller& controller, QWidget* parent)
    : QWidget(parent),
      controller_(controller),
      current_mode_(MainWindow::Mode::Polygons),
      is_drawing_polygon_(false),
      frame_count_(0),
      current_fps_(0.0)
{
    setMouseTracking(true);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
    current_mode_ = MainWindow::Mode::Polygons;
    fps_timer_.start();
}

void DrawingWidget::setIsDrawing(bool drawing) {
    if (is_drawing_polygon_ != drawing) {
        is_drawing_polygon_ = drawing;
        emit drawingStateChanged(is_drawing_polygon_);
    }
}

void DrawingWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::NoBrush);
    const auto& polygons = controller_.getPolygons();
    for (size_t i = 0; i < polygons.size(); ++i) {
         if (i == 0) continue;
        const auto& poly = polygons[i];
        const auto& vertices = poly.getVertices();
        if (vertices.size() >= 3) {
            painter.drawPolygon(vertices.data(), static_cast<int>(vertices.size()));
        }
    }

    painter.setPen(QPen(Qt::magenta, 1, Qt::DashLine));
    const auto& current_poly_opt = controller_.getCurrentPolygon();
    if (is_drawing_polygon_ && current_poly_opt.has_value()) {
        const auto& current_poly = current_poly_opt.value();
        const auto& vertices = current_poly.getVertices();
        if (vertices.size() > 0) {
            if (vertices.size() > 1) {
                 painter.drawPolyline(vertices.data(), static_cast<int>(vertices.size()));
                 painter.drawLine(vertices[vertices.size() - 2], last_mouse_pos_);
            } else if (vertices.size() == 1) {
                 painter.drawLine(vertices[0], last_mouse_pos_);
            }
        }
    }

    painter.setPen(Qt::NoPen);
    qreal lightSourceRadius = 4.0;
    painter.setBrush(QColor(255, 255, 0));
    for (const auto& source : controller_.getLightSources()) {
        painter.drawEllipse(source, lightSourceRadius, lightSourceRadius);
    }
    painter.setBrush(QColor(255, 0, 255));
    for (const auto& source : controller_.getStaticLightSources()) {
        painter.drawEllipse(source, lightSourceRadius, lightSourceRadius);
    }

    painter.setPen(Qt::NoPen);

    QColor dynamicLightAreaColor(255, 255, 255, 40);
    std::vector<Polygon> dynamic_light_areas = controller_.createDynamicLightAreas();
    painter.setBrush(QBrush(dynamicLightAreaColor));
    for (const auto& light_poly : dynamic_light_areas) {
        const auto& vertices = light_poly.getVertices();
        if (vertices.size() >= 3) {
            painter.drawPolygon(vertices.data(), static_cast<int>(vertices.size()));
        }
    }

    QColor staticLightAreaColor(255, 180, 255, 40);
    std::vector<Polygon> static_light_areas = controller_.createStaticLightAreas();
    painter.setBrush(QBrush(staticLightAreaColor));
    for (const auto& light_poly : static_light_areas) {
        const auto& vertices = light_poly.getVertices();
        if (vertices.size() >= 3) {
            painter.drawPolygon(vertices.data(), static_cast<int>(vertices.size()));
        }
    }

    frame_count_++;
    qint64 elapsed = fps_timer_.elapsed();
    if (elapsed >= 500) {
        current_fps_ = frame_count_ * 1000.0 / elapsed;
        frame_count_ = 0;
        fps_timer_.restart();
    }

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10));
    painter.drawText(10, 20, QString("FPS: %1").arg(current_fps_, 0, 'f', 1));

}

void DrawingWidget::mousePressEvent(QMouseEvent* event) {
    last_mouse_pos_ = event->pos();
    bool objectListChanged = false;

    if (current_mode_ == MainWindow::Mode::Polygons) {
        if (event->button() == Qt::LeftButton) {
            if (!is_drawing_polygon_) {
                controller_.startNewPolygon(last_mouse_pos_);
                setIsDrawing(true);
            } else {
                controller_.addVertexToCurrentPolygon(last_mouse_pos_);
            }
        } else if (event->button() == Qt::RightButton) {
            if (is_drawing_polygon_) {
                bool success = controller_.finalizeCurrentPolygon();
                objectListChanged = true;
                setIsDrawing(false);
                if (!success) {
                    QMessageBox::warning(this, "Polygon Creation Failed",
                                         "Could not add polygon.\n"
                                         "Reason: Less than 3 vertices, collision with another polygon, or contains a light source.");
                }
            }
        }
    } else if (current_mode_ == MainWindow::Mode::Light) {
        if (event->buttons() & Qt::LeftButton) {
            if (!controller_.setLightSourceCenter(last_mouse_pos_)) {
                 QMessageBox::warning(this, "Move Denied", "Cannot move light source inside a polygon.");
            }
        }
    } else if (current_mode_ == MainWindow::Mode::StaticLights) {
        if (event->button() == Qt::LeftButton) {
            if (controller_.addStaticLightSource(last_mouse_pos_)) {
                 objectListChanged = true;
            } else {
                 QMessageBox::warning(this, "Add Denied", "Cannot add static light source inside a polygon.");
            }
        }
    }

    if (objectListChanged) {
         emit objectsChanged();
    }
    update();
}

void DrawingWidget::mouseMoveEvent(QMouseEvent* event) {
    last_mouse_pos_ = event->pos();
    if (current_mode_ == MainWindow::Mode::Polygons) {
        if (is_drawing_polygon_) {
            controller_.updateCurrentPolygonLastVertex(last_mouse_pos_);
            update();
        }
    } else if (current_mode_ == MainWindow::Mode::Light) {
        if (event->buttons() & Qt::LeftButton) {
            controller_.setLightSourceCenter(last_mouse_pos_);
            update();
        }
    }
    QWidget::mouseMoveEvent(event);
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent* event) {
    QWidget::mouseReleaseEvent(event);
}

void DrawingWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    update();
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      current_mode_(Mode::Polygons),
      is_drawing_polygon_(false)
{
    setupUi();
    setupConnections();
    initializeScene();
    setMinimumSize(1200, 700);
}

void MainWindow::setupUi() {
    central_widget_ = new QWidget(this);
    main_layout_ = new QHBoxLayout(central_widget_);

    left_panel_widget_ = new QWidget(this);
    left_panel_layout_ = new QVBoxLayout(left_panel_widget_);
    left_panel_widget_->setLayout(left_panel_layout_);
    left_panel_widget_->setMinimumWidth(200);
    left_panel_widget_->setMaximumWidth(300);

    mode_combo_box_ = new QComboBox(left_panel_widget_);
    mode_combo_box_->addItem("Draw Polygons");
    mode_combo_box_->addItem("Move Dynamic Light");
    mode_combo_box_->addItem("Add Static Lights");
    mode_combo_box_->setToolTip("Select interaction mode");

    object_list_widget_ = new QListWidget(left_panel_widget_);
    object_list_widget_->setSelectionMode(QAbstractItemView::SingleSelection);

    delete_button_ = new QPushButton("Delete Selected", left_panel_widget_);

    left_panel_layout_->addWidget(mode_combo_box_);
    left_panel_layout_->addWidget(object_list_widget_, 1);
    left_panel_layout_->addWidget(delete_button_);

    drawing_widget_ = new DrawingWidget(controller_, this);
    drawing_widget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    main_layout_->addWidget(left_panel_widget_);
    main_layout_->addWidget(drawing_widget_, 1);

    central_widget_->setLayout(main_layout_);
    setCentralWidget(central_widget_);
    setWindowTitle("2D Raycaster Lab");
}

void MainWindow::setupConnections() {
    connect(mode_combo_box_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::modeChanged);
    connect(drawing_widget_, &DrawingWidget::drawingStateChanged,
            this, &MainWindow::onDrawingStateChanged);
    connect(delete_button_, &QPushButton::clicked,
            this, &MainWindow::onDeleteSelectedObject);
    connect(drawing_widget_, &DrawingWidget::objectsChanged,
            this, &MainWindow::updateObjectList);
}

void MainWindow::initializeScene() {
    controller_.addBoundaryPolygon(-10000, -10000, 10000, 10000);
    QPointF initial_light_pos(this->width() / 3.0, this->height() / 2.0);
    if (initial_light_pos.x() < 10 || initial_light_pos.y() < 10 || !isVisible()) {
        initial_light_pos = QPointF(600, 350);
    }
    controller_.setLightSourceCenter(initial_light_pos);
    updateObjectList();
}

void MainWindow::modeChanged(int index) {
    Mode new_mode = static_cast<Mode>(index);
    if (current_mode_ != new_mode) {
        if (current_mode_ == Mode::Polygons && this->is_drawing_polygon_) {
            controller_.finalizeCurrentPolygon(); 
            updateObjectList();
            drawing_widget_->setIsDrawing(false);
        }
        current_mode_ = new_mode;
        drawing_widget_->setMode(current_mode_);
        drawing_widget_->update();
    }
}

void MainWindow::onDrawingStateChanged(bool isDrawing) {
    this->is_drawing_polygon_ = isDrawing;
    if (!isDrawing) {
        updateObjectList();
    }
}

void MainWindow::updateObjectList() {
    object_list_widget_->clear();
    size_t poly_count = controller_.getPolygonCount();
    for (size_t i = 0; i < poly_count; ++i) {
        QListWidgetItem* item = new QListWidgetItem(QString("Polygon %1").arg(i + 1), object_list_widget_);
        item->setData(ObjectTypeRole, 0);
        item->setData(ObjectIndexRole, static_cast<int>(i));
    }
    size_t static_light_count = controller_.getStaticLightSourceCount();
    for (size_t i = 0; i < static_light_count; ++i) {
        QListWidgetItem* item = new QListWidgetItem(QString("Static Light %1").arg(i + 1), object_list_widget_);
        item->setData(ObjectTypeRole, 1);
        item->setData(ObjectIndexRole, static_cast<int>(i));
    }
}

void MainWindow::onDeleteSelectedObject() {
    QListWidgetItem* selected_item = object_list_widget_->currentItem();
    if (!selected_item) {
         QMessageBox::information(this, "Delete Object", "Please select an object from the list first.");
         return;
    }

    int type = selected_item->data(ObjectTypeRole).toInt();
    int index = selected_item->data(ObjectIndexRole).toInt();
    bool deleted = false;
    QString object_name = selected_item->text();

    if (type == 0) {
        deleted = controller_.removePolygon(static_cast<size_t>(index));
    } else if (type == 1) {
        deleted = controller_.removeStaticLightSource(static_cast<size_t>(index));
    }

    if (deleted) {
        updateObjectList();
        drawing_widget_->update();
    } else {
         QMessageBox::warning(this, "Delete Error", QString("Could not delete %1 (internal error or invalid index).").arg(object_name));
    }
}