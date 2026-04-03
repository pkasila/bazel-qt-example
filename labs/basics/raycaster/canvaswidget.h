#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include "controller.h"

#include <QWidget>
#include <QSizeF>
#include <QPainter>
#include <QSizePolicy>


enum class InteractionMode {
    Polygons,
    Light
};

class CanvasWidget : public QWidget
{
    Q_OBJECT;
public:
    explicit CanvasWidget(QWidget* parent = nullptr);

    void setMode(InteractionMode mode);
    void setController(Controller* cntrl);
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override ;
    void drawPolygons(QPainter& painter);
    void drawLightSource(QPainter& painter);
    bool drawingPolygon = false;

private:
    InteractionMode current_mode = InteractionMode::Polygons;
    Controller* controller = nullptr;

};

#endif // CANVASWIDGET_H
