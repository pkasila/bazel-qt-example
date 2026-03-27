#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "controller.h"
#include "ray.h"
#include "canvas.h"

#include <QMainWindow>
#include <QComboBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QComboBox* mode;
    Canvas* canva;
    void connections();
};
#endif // MAINWINDOW_H
