#include "ImageView.h"

#include <QPainter>
#include <QResizeEvent>
#include <QWidget>

ImageView::ImageView(const QUrl& image_url, QWidget* parent)
    : QGraphicsView(parent)
    , scene_(new QGraphicsScene(this))
    , pixmap_item_(scene_->addPixmap(QPixmap(image_url.toLocalFile()))) {
    setScene(scene_);
    setAlignment(Qt::AlignCenter);
    setResizeAnchor(AnchorViewCenter);
    setViewport(new QWidget());
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

void ImageView::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    fitInView(scene_->sceneRect(), Qt::KeepAspectRatio);
}
