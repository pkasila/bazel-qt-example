#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "controller.h"

#include <QMainWindow>
#include <QComboBox>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int GetMode() const {
        return mode->currentIndex();
    }

public slots:
    void ChangeMode(int index) {
        controller->SetMode(index);
    }

private:
    QComboBox* mode;
    Controller* controller;
};
#endif // MAINWINDOW_H
