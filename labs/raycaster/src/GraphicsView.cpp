#include "GraphicsView.h"

#include <QGraphicsView>
#include <QMouseEvent>
#include <QWidget>

GraphicsView::GraphicsView(QWidget* parent) : QGraphicsView(parent) {
    setMouseTracking(true);
}

void GraphicsView::mousePressEvent(QMouseEvent* event) {
    QGraphicsView::mousePressEvent(event);
    emit MousePressed(mapToScene(event->pos()), event->button());
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event) {
    QGraphicsView::mouseMoveEvent(event);
    emit MouseMoved(mapToScene(event->pos()));
}

void GraphicsView::resizeEvent(QResizeEvent* event) {
    const QSizeF old_size = event->oldSize();
    const QSizeF new_size = event->size();

    if (old_size.width() <= 0 || old_size.height() <= 0) {
        QGraphicsView::resizeEvent(event);
        return;
    }

    if (scene()) {
        scene()->setSceneRect(0, 0, event->size().width(), event->size().height());
    }

    const QPointF scale = {
      static_cast<double>(new_size.width()) / old_size.width(),
      static_cast<double>(new_size.height()) / old_size.height()};
    emit ViewScaled(scale);
    QGraphicsView::resizeEvent(event);
}
