#pragma once

#include <QMainWindow>

class QComboBox;

class CanvasWidget;

class MainWindow : public QMainWindow {
public:
    MainWindow();

private:
    QComboBox* mode_combo_;
    CanvasWidget* canvas_;
};
