#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMenuBar>
#include <QAction>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    //ui = new Ui::MainWindow;
    //ui->setupUi(this);
    setStyleSheet("QMainWindow {background-color: #999999}");
    resize(1080, 720);
    setMinimumHeight(500);
    setMinimumWidth(700);
    setWindowTitle("Padouble");

    overCanvas = new OverCanvas(this);
    overCanvas->setFixedSize(QSize(640, 480));
    toolBar = new PaintToolBar(this, overCanvas->getCanvas());

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *toolBarLayout = new QHBoxLayout;
    QVBoxLayout *canvasVLayout = new QVBoxLayout;
    QHBoxLayout *canvasLayout = new QHBoxLayout;

    toolBarLayout->addWidget(toolBar);
    toolBarLayout->setContentsMargins(0, 0, 0, 0);
    canvasVLayout->addWidget(overCanvas);
    canvasVLayout->addStretch(1);
    canvasLayout->addLayout(canvasVLayout);
    canvasLayout->addStretch(1);

    layout->addLayout(toolBarLayout);
    layout->addLayout(canvasLayout);
    layout->addStretch(1);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    centralWidget->setLayout(layout);

    QMenu* fileMenu = menuBar() -> addMenu(tr("&File"));
    QAction* openAction = new QAction(tr("&Open"), this);
    QAction* saveAction = new QAction(tr("&Save"), this);

    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);

    connect(openAction, &QAction::triggered, this, [this](){
        QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg *.jpeg)");
        if (!fileName.isEmpty()) {
            overCanvas->openImage(fileName);
        }
    });
    connect(saveAction, &QAction::triggered, this, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "Images (*.png *.jpg *.jpeg)");
        if (!fileName.isEmpty()) {
            overCanvas->saveImage(fileName);
        }
    });
}

MainWindow::~MainWindow()
{
    //delete ui;
}
