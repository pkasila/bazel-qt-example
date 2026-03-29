#include "mainwindow.h"
#include "board.h"
#include <QDebug>

mainwindow::mainwindow(QWidget *parent)
    : QMainWindow(parent)
{   
    QWidget *centralWidget = new QWidget(this);// NOLINT
    setCentralWidget(centralWidget); 
    
    mainLayout = new QVBoxLayout(centralWidget);// NOLINT

    // Верхняя часть
    topLayout = new QHBoxLayout(); // NOLINT
    button1 = new QRadioButton("Light",this); // NOLINT
    button2 = new QRadioButton("Polygons",this); // NOLINT
    button3 = new QRadioButton("Static lights",this); // NOLINT
    topLayout->addWidget(button1);
    topLayout->addWidget(button2);
    topLayout->addWidget(button3);
    // Нижняя часть 
    bottomLayout = new QVBoxLayout();// NOLINT
    mainboard = new board();//NOLINT
    
    QPalette pal = mainboard->palette();
    pal.setColor(QPalette::Window, QColor(189, 154, 208));
    mainboard->setAutoFillBackground(true);
    mainboard->setPalette(pal);
    //mainboard->setStyleSheet("background-color:rgb(189, 154, 208);");

    mainboard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bottomLayout->addWidget(mainboard);

    mainLayout->addLayout(topLayout,1);
    mainLayout->addLayout(bottomLayout,3);
    mainboard->show();

    connect(button1, &QPushButton::clicked, this, [&](){mainboard->ctrl_.Mode() = 0;});
    connect(button2, &QRadioButton::clicked, this, [&](){mainboard->ctrl_.Mode() = 1;mainboard->update();});
    connect(button3, &QRadioButton::clicked, this, [&](){mainboard->ctrl_.Mode() = 2;mainboard->update();});
    
}