#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPaintEvent>
#include <vector>
#include "controller.h"

class DrawingWidget : public QWidget {
public:
    explicit DrawingWidget(QWidget* parent = nullptr);
    
    void setController(Controller* controller) { controller_ = controller; }
    void setMode(const QString& mode) { mode_ = mode; }
    
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    
private:
    Controller* controller_;
    QString mode_;
    bool is_drawing_polygon_;
    
    void drawLightSource(QPainter& painter);
    void drawPolygons(QPainter& painter);
    void drawLightAndShadows(QPainter& painter);
    void drawLightAreas(QPainter& painter);
};

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget* parent = nullptr);
    
private:
    Controller* controller_;
    DrawingWidget* drawing_widget_;
    QComboBox* mode_combo_;
    QPushButton* clear_button_;
    QPushButton* add_light_button_;
    QLabel* status_label_;
    
    void setupUI();
};

#endif // MAIN_WINDOW_H
