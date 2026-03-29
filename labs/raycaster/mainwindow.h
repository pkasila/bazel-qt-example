#pragma once 
#include <QPushButton>
#include <QtWidgets>

#include "board.h"

class mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    mainwindow(QWidget *parent = nullptr);
private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *topLayout;
    QVBoxLayout *bottomLayout;

    board *mainboard;



    QRadioButton *button1;
    QRadioButton *button2;
    QRadioButton *button3;
};