//
// Created by blalex on 4/6/25.
//

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include <QComboBox>
#include <QMainWindow>
#include "drawing_widget.h"
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;


private:
    void SetupWindow();

    void ChangeMode(int mode);

    QComboBox* mode_selector_;
    DrawingWidget* drawing_widget_;
    Controller controller_;
    QLabel* fps_label_;
};
#endif  // MAIN_WINDOW_H
