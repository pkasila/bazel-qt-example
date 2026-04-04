#ifndef DRAWLAYER_H
#define DRAWLAYER_H
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "polygon.h"
#include "controller.h"

class DrawLayer : public QWidget
{
    Q_OBJECT
public:
    explicit DrawLayer(QWidget* parent = nullptr);
    void ChangeMode(QString mode);

protected:
    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;


private:
    bool new_polygon = true;
    Controller controller;
    QString mode = "Polygons";
};

#endif // DRAWLAYER_H
