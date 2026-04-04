#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QVBoxLayout>
#include <QPalette>
#include "drawlayer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    private slots:
        void on_comboBox_currentTextChanged(const QString &arg1);

private:
    QComboBox *comboBox;
    QVBoxLayout *verticalLayout;
    DrawLayer* layer;
};

#endif // MAINWINDOW_H
