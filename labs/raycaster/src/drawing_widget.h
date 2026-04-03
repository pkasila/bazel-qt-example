//
// Created by blalex on 4/8/25.
//

#ifndef DRAWING_WIDGET_H
#define DRAWING_WIDGET_H

#include "controller.h"
#include "drawing_widget.h"

#include <QMouseEvent>
// #include <QOpenGLWidget>
#include <QPainter>

#include <QWidget>
class DrawingWidget : public QWidget {

// class DrawingWidget : public QOpenGLWidget {
    Q_OBJECT
   public:
    explicit DrawingWidget(Controller* controller, QWidget* parent = nullptr);

   protected:
    void paintEvent(QPaintEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event);

    void mousePressEvent(QMouseEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

   signals:
    void FPSUpdated(const QString& fpsText);

   private:
    Controller* controller_;
    bool drawing_polygon_ = false;

    void FillPolygonArea(
        QPainter& painter, const std::vector<QPointF>& points, const QBrush& fill_color);
};

#endif  // DRAWING_WIDGET_H
