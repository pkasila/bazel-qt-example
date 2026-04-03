#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>

#include "controller.h"
#include "canvaswidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    void setupUi();

    QComboBox* modeChoose;
    Controller* controller;
    CanvasWidget* canvas;

private slots:
    void onModeChanged(int index);

};

#endif // MAINWINDOW_H
