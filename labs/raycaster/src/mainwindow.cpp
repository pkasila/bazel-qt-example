#include "mainwindow.h"
#include <QApplication>
#include <QComboBox>
#include <QVBoxLayout>
#include <QWidget>
#include "drawlayer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    verticalLayout = new QVBoxLayout(centralWidget);

    comboBox = new QComboBox(this);
    comboBox->addItem("Polygons");
    comboBox->addItem("Light");
    verticalLayout->addWidget(comboBox);

    layer = new DrawLayer(this);
    layer->setAutoFillBackground(true);
    QPalette pal = layer->palette();
    pal.setColor(QPalette::Window, Qt::black);
    layer->setPalette(pal);


    verticalLayout->addWidget(layer);

    connect(comboBox, &QComboBox::currentTextChanged, this, [=](const QString& text){
        layer->ChangeMode(text);
        layer->update();
    });

    resize(1000, 600);
}

MainWindow::~MainWindow()
{
    delete layer;
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    layer->ChangeMode(arg1);
}