#pragma once

#include <QMainWindow>

class DrawWidget;
class QComboBox;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    DrawWidget* draw_widget_ = nullptr;
    QComboBox* mode_combo_box_ = nullptr;

    void SetupUi();
    void SetupConnections();
};