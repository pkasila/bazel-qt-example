#include "../include/mainwindow.h"
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    setWindowTitle("Raycaster");

    lightMode = new QRadioButton("Light Mode", this);
    polygonsMode = new QRadioButton("Polygons Mode", this);

    m_drawingArea = new DrawingArea();

    connect(lightMode, &QRadioButton::toggled, [this](bool checked) {
        if (checked) {
            (*m_drawingArea).setMode(DrawingArea::Light);
        }
    });

    connect(polygonsMode, &QRadioButton::toggled, [this](bool checked) {
        if (checked) {
            (*m_drawingArea).setMode(DrawingArea::Polygons);
        }
    });

    QHBoxLayout *groupBoxLayout = new QHBoxLayout(this);
    groupBoxLayout->addWidget(lightMode);
    groupBoxLayout->addWidget(polygonsMode);

    layout->addLayout(groupBoxLayout);
    layout->addWidget(m_drawingArea);

    centralWidget->setLayout(layout);
    this->setCentralWidget(centralWidget);
};