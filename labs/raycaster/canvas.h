#ifndef CANVAS_H
#define CANVAS_H

#include "controller.h"
#include "utils.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPoint>
#include <QResizeEvent>
#include <QWidget>

class CanvasWidget : public QWidget {
   public:
    explicit CanvasWidget(QWidget* parent = nullptr);
    void setRenderMode(RenderMode newMode);

   protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

   private:
    QPoint convertToScene(const QPoint& widgetPos) const;
    RenderMode activeMode;
    RaycasterController controller;
    bool isDrawing;
    QPoint previewPt;
};

#endif  // CANVAS_H
