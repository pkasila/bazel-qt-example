#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QPointF>
#include <QWidget>

class GraphicsView final : public QGraphicsView {
    Q_OBJECT
   public:
    explicit GraphicsView(QWidget* parent = nullptr);

   signals:
    void MousePressed(QPointF pos, Qt::MouseButton button);
    void MouseMoved(QPointF pos);
    void ViewScaled(QPointF scale);

   protected:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void resizeEvent(QResizeEvent* event) override;
};

#endif  // GRAPHICSVIEW_H
