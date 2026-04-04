#pragma once

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>

namespace raycaster {

class DrawingWidget;

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    DrawingWidget* drawing_widget_ = nullptr;
    QComboBox* mode_combo_box_ = nullptr;
    QLabel* help_label_ = nullptr;
};

}  // namespace raycaster
