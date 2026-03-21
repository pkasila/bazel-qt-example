#include "mainwindow.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Raycaster");
    resize(1200, 750);

    auto* centralWidget = new QWidget();
    auto* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto* topPanel = new QHBoxLayout();
    topPanel->setContentsMargins(10, 5, 10, 5);

    auto* modeGroup = new QButtonGroup(this);

    rbLight = new QRadioButton("1: Основной свет");
    rbPoly = new QRadioButton("2: Рисование");
    rbStatic = new QRadioButton("3: Статичный свет");

    modeGroup->addButton(rbLight);
    modeGroup->addButton(rbPoly);
    modeGroup->addButton(rbStatic);
    rbLight->setChecked(true);

    cbToggle = new QCheckBox("L: Свет ВКЛ");
    cbToggle->setChecked(true);

    cbPhoto = new QCheckBox("P: Фото-режим");
    cbPhoto->setChecked(false);

    lblDetail = new QLabel("Детализация теней:");
    sliderDetail = new QSlider(Qt::Horizontal);
    sliderDetail->setRange(0, 16);
    sliderDetail->setValue(4);
    sliderDetail->setFixedWidth(100);

    btnHelp = new QPushButton("H: Помощь");
    btnHelp->setFixedWidth(120);

    topPanel->addWidget(rbLight);
    topPanel->addWidget(rbPoly);
    topPanel->addWidget(rbStatic);
    topPanel->addSpacing(20);
    topPanel->addWidget(lblDetail);
    topPanel->addWidget(sliderDetail);
    topPanel->addStretch();
    topPanel->addWidget(cbPhoto);
    topPanel->addWidget(cbToggle);
    topPanel->addWidget(btnHelp);

    canvas = new Canvas();
    mainLayout->addLayout(topPanel);
    mainLayout->addWidget(canvas, 1);
    setCentralWidget(centralWidget);

    connect(rbLight, &QRadioButton::toggled, [=](bool cs) {
        if (cs) {
            canvas->SetMode(Mode::Light);
        }
    });
    connect(rbPoly, &QRadioButton::toggled, [=](bool cs) {
        if (cs) {
            canvas->SetMode(Mode::Polygons);
        }
    });
    connect(rbStatic, &QRadioButton::toggled, [=](bool cs) {
        if (cs) {
            canvas->SetMode(Mode::StaticLights);
        }
    });

    connect(cbToggle, &QCheckBox::toggled, [=](bool v) { canvas->SetLightEnabled(v); });
    connect(cbPhoto, &QCheckBox::toggled, [=](bool v) { canvas->SetPhotoEnabled(v); });

    connect(
        sliderDetail, &QSlider::valueChanged, [=](int value) { canvas->SetShadowDetail(value); });

    connect(btnHelp, &QPushButton::clicked, this, &MainWindow::showHelp);
}

void MainWindow::showHelp() {
    QMessageBox::information(
        this, "Помощь / Управление",
        "Управление режимами (Хоткеи):\n"
        "[1] - Режим перемещения основного света.\n"
        "[2] - Режим рисования многоугольников.\n"
        "      ЛКМ: добавить вершину. ПКМ: закончить многоугольник.\n"
        "[3] - Режим установки статических источников.\n"
        "[L] - Включить/Выключить расчет света.\n"
        "[P] - Включить/Выключить режим фото-текстур.\n"
        "[H] - Показать это окно.\n\n"
        "[+] / [-] - Изменение детализации теней.");
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_1:
            rbLight->animateClick();
            break;
        case Qt::Key_2:
            rbPoly->animateClick();
            break;
        case Qt::Key_3:
            rbStatic->animateClick();
            break;
        case Qt::Key_P:
            cbPhoto->animateClick();
            break;
        case Qt::Key_L:
            cbToggle->animateClick();
            break;
        case Qt::Key_H:
            btnHelp->animateClick();
            break;

        case Qt::Key_Plus:
        case Qt::Key_Equal:
            sliderDetail->setValue(sliderDetail->value() + 1);
            break;

        case Qt::Key_Minus:
            sliderDetail->setValue(sliderDetail->value() - 1);
            break;

        default:
            QMainWindow::keyPressEvent(event);
    }
}