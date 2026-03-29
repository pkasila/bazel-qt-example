//
// Created by Zazerg on 3/31/25.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QButtonGroup>
#include <QMainWindow>
#include <QPainter>
#include <QSlider>
#include <QSpinBox>

#include "RenderArea.h"

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

private:
    RenderArea* render_area_;
    QButtonGroup* group;
    QSpinBox* red, *green, *blue;
    QSlider *red1, *green1, *blue1;
    Settings settings_;

public slots :
    void checked();

    void colorEdit();

    void colorEdit1();
};



#endif //MAINWINDOW_H
