//
// Created by Zazerg on 3/31/25.
//

#ifndef RENDERAREA_H
#define RENDERAREA_H

#include "Controller.h"
// #include "MainWindow.h"
#ifdef USE_OPENGL
#include <QtOpenGLWidgets/qopenglwidget.h>
#else
#include <QtWidgets/qwidget.h>
#endif

#include <QtCore/QPointF>
#include <QtCore/qtmetamacros.h>

#include "Settings.h"


class RenderArea : public
#ifdef USE_OPENGL
QOpenGLWidget
#else
QWidget
#endif
{
    Q_OBJECT
    public:
    RenderArea(Settings& settings, QWidget *parent = 0);
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;



    void drawPolygon(QPainter& painter, const Polygon& polygon);

    bool clipping_ = false;

    Controller& getController();

    private:
    Controller controller_;
    Settings& settings_;
    bool force_update_ = false;
    QPoint topleft_ = {100,100}, add_ = {100,100};
    double scale_ = 1;
    bool moving_ = false;
};



#endif //RENDERAREA_H
