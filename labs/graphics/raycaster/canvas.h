#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include "controller.h"
#include <QPainter>
#include <QMouseEvent>

class Canvas : public QWidget {
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = nullptr);
    void setPolygonMode(bool enabled);
    void setLightMode(bool enabled);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    enum Mode { PolygonMode, LightMode };
    Mode m_mode;
    Controller m_controller;

    void updateBoundingArea();
    void drawBoundingPolygon(QPainter &painter);
    void drawUserPolygons(QPainter &painter);
    void drawLight(QPainter &painter);
};



#endif // CANVAS_H
