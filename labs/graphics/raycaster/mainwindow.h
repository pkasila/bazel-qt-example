#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRadioButton>
#include "canvas.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    Canvas *m_canvas;
    QRadioButton *m_polygonModeBtn;
    QRadioButton *m_lightModeBtn;

    void setupUI();
};

#endif // MAINWINDOW_H
