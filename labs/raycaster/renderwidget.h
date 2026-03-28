#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include "controller.h"

enum class InteractionMode {
    Light,
    Polygons,
    StaticLights
};

class RenderWidget : public QWidget {
public:
    explicit RenderWidget(Controller* controller, QWidget *parent = nullptr);
    
    void setMode(InteractionMode mode) { mode_ = mode; update(); }
    InteractionMode getMode() const { return mode_; }
    void clearAll();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void drawScene(QPainter& painter);
    void drawPolygons(QPainter& painter);
    void drawLightSources(QPainter& painter);
    void drawLightAreas(QPainter& painter);
    void drawCurrentPolygon(QPainter& painter);
    
    Controller* controller_;
    InteractionMode mode_;
    bool is_drawing_polygon_;
    QPoint mouse_position_;
};

#endif // RENDERWIDGET_H
