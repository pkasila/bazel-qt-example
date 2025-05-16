#pragma once

#include <QMainWindow>
#include <QComboBox>
#include <QVBoxLayout>
#include "Canvas.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Canvas* canvas_;
    QComboBox* modeComboBox_;
    QWidget* centralWidget_;
    QVBoxLayout* mainLayout_;
};
