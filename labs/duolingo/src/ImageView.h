#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>

class ImageView : public QGraphicsView {
    Q_OBJECT
   public:
    ImageView(const QUrl& image_url, QWidget* parent = nullptr);

   protected:
    void resizeEvent(QResizeEvent* event) override;

   private:
    QGraphicsScene* scene_;
    QGraphicsPixmapItem* pixmap_item_;
};

#endif  // IMAGEVIEW_H
