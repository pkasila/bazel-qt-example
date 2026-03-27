#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    mode = new QComboBox(this);
    mode->addItems({"Light", "Polygons", "Bounded Lights"});
    mode->setFixedHeight(30);

    canva = new Canvas(this);
    canva->setFixedSize(790, 532);

    mainLayout->addWidget(mode);
    mainLayout->addWidget(canva, 1);

    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    setCentralWidget(centralWidget);
    connections();
}

void MainWindow::connections() {
    connect(mode, &QComboBox::currentTextChanged, this, [this]() {
        if (mode->currentText() == "Light") {
            this->canva->getC().mode = 0;
        } else if (mode->currentText() == "Polygons"){
            this->canva->getC().mode = 1;
        } else {
            this->canva->getC().mode = 2;
        }/* else {
            this->canva->getC().mode = 1;
        }*/
    });
}

MainWindow::~MainWindow()
{
}
