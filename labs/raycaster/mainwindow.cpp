#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QGroupBox *modeGroup = new QGroupBox("Режимы работы", this);
    QHBoxLayout *modeLayout = new QHBoxLayout(modeGroup);

    m_polygonModeBtn = new QRadioButton("Редактирование полигонов", this);
    m_lightModeBtn = new QRadioButton("Режим освещения", this);

    modeLayout->addWidget(m_polygonModeBtn);
    modeLayout->addWidget(m_lightModeBtn);

    m_polygonModeBtn->setAutoExclusive(false);
    m_lightModeBtn->setAutoExclusive(false);

    connect(m_polygonModeBtn, &QRadioButton::toggled, [this](bool checked) {
        if (checked) m_lightModeBtn->setChecked(false);
    });

    connect(m_lightModeBtn, &QRadioButton::toggled, [this](bool checked) {
        if (checked) m_polygonModeBtn->setChecked(false);
    });

    modeGroup->setLayout(modeLayout);

    m_canvas = new Canvas(this);

    mainLayout->addWidget(modeGroup);
    mainLayout->addWidget(m_canvas, 1);

    setCentralWidget(centralWidget);
    resize(1000, 700);
    setWindowTitle("RayCaster");

    connect(m_polygonModeBtn, &QRadioButton::toggled,
            m_canvas, &Canvas::setPolygonMode);
    connect(m_lightModeBtn, &QRadioButton::toggled,
            m_canvas, &Canvas::setLightMode);
}
