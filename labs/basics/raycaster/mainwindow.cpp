#include "mainwindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), controller(new Controller())
{
    setupUi();
}

MainWindow::~MainWindow() {
    delete controller;
}

void MainWindow::setupUi() {
    setFixedSize(900, 750);
    setWindowTitle("RayCaster");

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    modeChoose = new QComboBox();
    modeChoose->addItem("Polygons");
    modeChoose->addItem("Light");

    canvas = new CanvasWidget();
    canvas->setController(controller);

    connect(modeChoose, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModeChanged);

    onModeChanged(0);

    layout->addWidget(modeChoose);
    layout->addWidget(canvas, 1);

    centralWidget->setLayout(layout);
}

void MainWindow::onModeChanged(int index) {
    if (!canvas) return;
    if (index == 0) {
        canvas->setMode(InteractionMode::Polygons);
    } else {
        canvas->setMode(InteractionMode::Light);
    }
    canvas->drawingPolygon = false;
}


