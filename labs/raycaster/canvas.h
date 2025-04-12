#ifndef CANVAS_H
#define CANVAS_H

#include "controller.h"

#include <QWidget>
#include <QPointF>

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = nullptr);
    void setC(const Controller& c) {
        controller = c;
    }

    Controller& getC() { return controller; }
signals:

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
   // void mouseDoubleClickEvent(QMouseEvent* event) override;
private:
    Controller controller;
    //void DrawPolygons();
    bool isNew = true;
    bool isNewB = true;
    void FillPolygon(Polygon& polygon, QPainter& painter);
    QPointF s;
};

#endif // CANVAS_H
