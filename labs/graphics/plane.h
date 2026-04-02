#ifndef PLANE_H
#define PLANE_H

#include "controller.h"


#include <QWidget>
#include <QImage>
#include <QPoint>
#include <QColor>

class MainWindow;

class Plane : public QWidget{
public:
    Plane(QWidget *parent = nullptr);
    ~Plane();
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    friend class MainWindow;
private:
    Controller controller_;
};

#endif  // PLANE_H
