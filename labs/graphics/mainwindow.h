#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "plane.h"

#include <QMainWindow>


class MainWindow : public QMainWindow {
    Q_OBJECT //NOLINT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
private:
    Plane *plane_;
};
#endif  // MAINWINDOW_H
