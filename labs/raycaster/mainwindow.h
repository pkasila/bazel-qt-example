#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "controller.h"
#include "renderwidget.h"

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void onModeChanged();
    void onClearClicked();

private:
    void setupUI();
    void setupLayout();

    QWidget* central_widget_;
    RenderWidget* render_widget_;
    QComboBox* mode_combo_;
    QPushButton* clear_button_;
    QLabel* status_label_;

    QVBoxLayout* main_layout_;
    QHBoxLayout* control_layout_;

    Controller controller_;
};

#endif // MAINWINDOW_H
