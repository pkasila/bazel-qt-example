#include "../input/mainwindow.h"
#include "../input/lifewidget.h"

#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QWidget>
#include <QTimer>
#include <QGroupBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), isRunning(false)
{
    timer = new QTimer(this);
    timer->setInterval(200);

    setupUi();
    connectSignalsSlots();

    setWindowTitle("Qt Game of Life");
    resize(800, 650);
}

MainWindow::~MainWindow()
{
    if (timer && timer->isActive()) {
        timer->stop();
    }
}

void MainWindow::setupUi()
{
    lifeWidget = new LifeWidget(30, 30, this);
    startButton = new QPushButton("Start", this);
    stepButton = new QPushButton("Next Step", this);
    clearButton = new QPushButton("Clear", this);
    generationLabel = new QLabel("Generation: 0", this);
    speedSlider = new QSlider(Qt::Horizontal, this);
    speedSpinBox = new QSpinBox(this);

    rowsSpinBox = new QSpinBox(this);
    colsSpinBox = new QSpinBox(this);
    resizeButton = new QPushButton("Apply Size", this);

    rowsSpinBox->setRange(5, 500);
    colsSpinBox->setRange(5, 500);
    rowsSpinBox->setValue(lifeWidget->rows());
    colsSpinBox->setValue(lifeWidget->cols());
    rowsSpinBox->setToolTip("Number of rows in the grid");
    colsSpinBox->setToolTip("Number of columns in the grid");
    resizeButton->setToolTip("Resize the grid (clears the field)");

    speedSlider->setRange(10, 1000);
    speedSlider->setValue(timer->interval());
    speedSlider->setToolTip("Simulation Speed (Timer Interval ms)");
    speedSpinBox->setRange(10, 1000);
    speedSpinBox->setValue(timer->interval());
    speedSpinBox->setSuffix(" ms");
    speedSpinBox->setToolTip("Simulation Speed (Timer Interval ms)");
    speedSpinBox->setFixedWidth(100);

    QFormLayout *sizeFormLayout = new QFormLayout;
    sizeFormLayout->addRow("Rows:", rowsSpinBox);
    sizeFormLayout->addRow("Cols:", colsSpinBox);

    QVBoxLayout *sizeControlLayout = new QVBoxLayout;
    sizeControlLayout->addLayout(sizeFormLayout);
    sizeControlLayout->addWidget(resizeButton);

    QGroupBox *sizeGroup = new QGroupBox("Grid Size");
    sizeGroup->setLayout(sizeControlLayout);

    QVBoxLayout *controlPanelLayout = new QVBoxLayout;
    controlPanelLayout->addWidget(startButton);
    controlPanelLayout->addWidget(stepButton);
    controlPanelLayout->addWidget(clearButton);
    controlPanelLayout->addWidget(generationLabel);
    controlPanelLayout->addSpacing(10);
    controlPanelLayout->addWidget(new QLabel("Speed:", this));
    controlPanelLayout->addWidget(speedSlider);
    controlPanelLayout->addWidget(speedSpinBox);
    controlPanelLayout->addSpacing(10);
    controlPanelLayout->addWidget(sizeGroup);
    controlPanelLayout->addStretch();

    QWidget *controlWidget = new QWidget();
    controlWidget->setLayout(controlPanelLayout);
    controlWidget->setMinimumWidth(180);
    controlWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    mainLayout->addWidget(lifeWidget, 1);
    mainLayout->addWidget(controlWidget, 0);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

void MainWindow::connectSignalsSlots()
{
    connect(startButton, &QPushButton::clicked, this, &MainWindow::toggleSimulation);
    connect(stepButton, &QPushButton::clicked, this, &MainWindow::stepOnce);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearGrid);

    connect(speedSlider, &QSlider::valueChanged, speedSpinBox, &QSpinBox::setValue);
    connect(speedSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), speedSlider, &QSlider::setValue);

    connect(speedSlider, &QSlider::valueChanged, this, &MainWindow::updateSpeed);

    connect(timer, &QTimer::timeout, lifeWidget, &LifeWidget::nextGeneration);

    connect(lifeWidget, &LifeWidget::generationChanged, this, &MainWindow::updateGenerationLabel);

    connect(resizeButton, &QPushButton::clicked, this, &MainWindow::applyNewGridSize);
}

void MainWindow::toggleSimulation()
{
    if (isRunning) {
        timer->stop();
        startButton->setText("Start");
        stepButton->setEnabled(true);
        rowsSpinBox->setEnabled(true);
        colsSpinBox->setEnabled(true);
        resizeButton->setEnabled(true);
    } else {
        rowsSpinBox->setEnabled(false);
        colsSpinBox->setEnabled(false);
        resizeButton->setEnabled(false);
        updateSpeed(speedSlider->value());
        timer->start();
        startButton->setText("Stop");
        stepButton->setEnabled(false);
    }
    isRunning = !isRunning;
}

void MainWindow::stepOnce()
{
    if (!isRunning) {
        lifeWidget->nextGeneration();
    }
}

void MainWindow::clearGrid()
{
    if (isRunning) {
        toggleSimulation();
    }
    lifeWidget->clearGrid();
}

void MainWindow::updateSpeed(int value)
{
    timer->setInterval(value);
}

void MainWindow::updateGenerationLabel(int generation)
{
    generationLabel->setText(QString("Generation: %1").arg(generation));
}

void MainWindow::applyNewGridSize()
{
    if (isRunning) {
       toggleSimulation();
    }
    int newRows = rowsSpinBox->value();
    int newCols = colsSpinBox->value();
    lifeWidget->resizeGrid(newRows, newCols);
}