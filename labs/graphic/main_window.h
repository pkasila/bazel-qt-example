#pragma once

#include <QMainWindow>

class CanvasWidget;
class QLabel;
class QComboBox;

class MainWindow : public QMainWindow {
public:
    MainWindow();

private:
    CanvasWidget* canvas_;
    QComboBox* mode_box_;
    QLabel* help_label_;
};
