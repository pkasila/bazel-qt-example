#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QWidget>
#include "controller.h"

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void modeChanged(int index);

    Controller controller_;
    QComboBox *modeSelector_;
    QWidget *drawArea_;
    bool drawingPolygon_;
    enum Mode { LIGHT, POLYGONS } currentMode_;
};

#endif