#include "mainwindow.h"

#include <vector>
#include <QPointF>
#include <QVBoxLayout>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mode(new QComboBox(this))
    , controller(new Controller)
{
    ui->setupUi(this);

    QWidget* control = new QWidget;
    this->setCentralWidget(control);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(mode);
    layout->addWidget(controller);
    control->setLayout(layout);
    repaint();

    mode->addItem("Polygons");
    mode->addItem("Light");

    connect(mode, &QComboBox::activated, this, &MainWindow::ChangeMode);
}

MainWindow::~MainWindow()
{
    delete ui;
}
