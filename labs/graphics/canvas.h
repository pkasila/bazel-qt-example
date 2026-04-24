#pragma once
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QKeyEvent>
#include "controller.h"

enum class Mode { Polygons, Light };

class Canvas : public QWidget {
public:
    Canvas(QWidget* parent = nullptr);
    void setMode(Mode mode);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    Controller m_controller;
    Mode m_mode = Mode::Polygons;
    bool m_drawing = false;
};