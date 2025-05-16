#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Light Simulation");

    centralWidget_ = new QWidget(this);
    setCentralWidget(centralWidget_);

    mainLayout_ = new QVBoxLayout(centralWidget_);

    modeComboBox_ = new QComboBox(this);
    modeComboBox_->addItem("Light");
    modeComboBox_->addItem("Polygons");
    mainLayout_->addWidget(modeComboBox_);

    canvas_ = new Canvas(this);
    canvas_->SetMode(Mode::Light);
    mainLayout_->addWidget(canvas_);

    connect(modeComboBox_, &QComboBox::currentTextChanged, this, [=](const QString& text){
        if (text == "Light")
            canvas_->SetMode(Mode::Light);
        else
            canvas_->SetMode(Mode::Polygons);

        canvas_->update();
    });

    resize(800, 600);
}

MainWindow::~MainWindow()
{
}
