#ifndef OVERCANVAS_H
#define OVERCANVAS_H


#include "canvas.h"
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QColor>
#include <QPoint>
#include <QTimer>
#include <QPushButton>

class OverCanvas : public QWidget {
    Q_OBJECT
public:
    explicit OverCanvas(QWidget *parent = nullptr);
    Canvas* getCanvas();
    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName);
    ~OverCanvas();
protected:
     void mousePressEvent(QMouseEvent *event) override;
     void mouseMoveEvent(QMouseEvent *event) override;
     void mouseReleaseEvent(QMouseEvent *event) override;
    // void paintEvent(QPaintEvent *event) override;
    // void resizeEvent(QResizeEvent *event) override;
private:

     class ColoredWidget : public QWidget {
     public:
         ColoredWidget(const QColor &color, QWidget *parent = nullptr)
             : QWidget(parent), color(color) {
             setFixedSize(10, 10);
         }
         friend class OverCanvas;
     protected:
         void paintEvent(QPaintEvent *event) override {
             QPainter painter(this);
             painter.fillRect(rect(), color);
         }

     private:
         QColor color;
     };
    QWidget *itsParent;
    Canvas *canvas;
    ColoredWidget *bottomButton;
    ColoredWidget *rightmButton;
    bool resizeY = false;
    bool resizeX = false;
    QPoint lastPoint;
};

#endif // OVERCANVAS_H
