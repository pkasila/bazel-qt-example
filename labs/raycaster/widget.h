#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPainter>
#include <QPainterPath>
#include "controller.h"
#include "mode.h"

class DrawingArea : public QWidget {
public:
    explicit DrawingArea(QWidget* parent = nullptr);
    ~DrawingArea() override;
    
    void setController(Controller* ctrl) { controller_ = ctrl; }
    void setMode(Mode mode) { current_mode_ = mode; update(); }
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    
private:
    void drawPolygons(QPainter& painter);
    void drawLight(QPainter& painter);
    
    Controller* controller_ = nullptr;
    Mode current_mode_ = Mode::Polygons;
    bool building_polygon_ = false;
};

class RaycasterWidget : public QWidget {
public:
    explicit RaycasterWidget(QWidget* parent = nullptr);
    ~RaycasterWidget() override;
    
private:
    void initUI();
    void switchMode(Mode mode);
    
    Controller controller_;
    DrawingArea* drawing_area_ = nullptr;
    QComboBox* mode_combo_ = nullptr;
    QVBoxLayout* layout_ = nullptr;
};