#pragma once

#include "../input/controller.h"
#include <QComboBox>
#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QListWidget>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPointF>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

class QComboBox;
class QListWidget;
class QPushButton;
class DrawingWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    enum class Mode {
        Polygons = 0,
        Light = 1,
        StaticLights = 2
    };

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void modeChanged(int index);
    void onDrawingStateChanged(bool isDrawing);
    void updateObjectList();
    void onDeleteSelectedObject();

private:
    Controller controller_;
    Mode current_mode_;
    bool is_drawing_polygon_;

    QWidget* central_widget_;
    QHBoxLayout* main_layout_;
    QVBoxLayout* left_panel_layout_;
    QWidget* left_panel_widget_;
    QComboBox* mode_combo_box_;
    QListWidget* object_list_widget_;
    QPushButton* delete_button_;
    DrawingWidget* drawing_widget_;

    void setupUi();
    void setupConnections();
    void initializeScene();
};

class DrawingWidget : public QWidget {
    Q_OBJECT

public:
    explicit DrawingWidget(Controller& controller, QWidget* parent = nullptr);
    void setMode(MainWindow::Mode mode) { current_mode_ = mode; update(); }
    void setIsDrawing(bool drawing);
    bool isDrawing() const { return is_drawing_polygon_; }

signals:
    void drawingStateChanged(bool isDrawing);
    void objectsChanged();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    Controller& controller_;
    MainWindow::Mode current_mode_;
    bool is_drawing_polygon_;
    QPointF last_mouse_pos_;

    QElapsedTimer fps_timer_;
    int frame_count_;
    double current_fps_;

    QPolygonF toQPolygonF(const std::vector<QPointF>& vertices) const;
};