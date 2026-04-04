#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QComboBox>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPushButton>
#include "raycaster_controller.h"

class RenderWidget : public QWidget {
    Q_OBJECT
public:
    explicit RenderWidget(QWidget* parent = nullptr);
    
    enum class Mode { Light, Polygons };
    void setMode(Mode mode);

    void clearPolygons();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    Controller m_controller;
    Mode m_mode = Mode::Light;
    bool m_isDrawingPolygon = false;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onModeChanged(int index);
    void onClearClicked();

private:
    QComboBox* modeComboBox;
    QPushButton* clearButton; 
    RenderWidget* renderWidget;
};