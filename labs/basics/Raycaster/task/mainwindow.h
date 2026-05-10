#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include "drawing_widget.h"

class MainWindow : public QMainWindow {
   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

   private:
    QComboBox* mode_combo_box_;
    DrawingWidget* drawing_widget_;
};

#endif  // MAINWINDOW_H