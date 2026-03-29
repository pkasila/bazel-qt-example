#pragma once
#include <QtWidgets>
#include <QPainterPath>
#include "controller.h"

class board : public QWidget {
    Q_OBJECT
   public:
    explicit board(QWidget* parent = nullptr);

    Controller ctrl_;
   protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;

   private:
};