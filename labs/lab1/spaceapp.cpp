#include "spaceapp.h"
#include <QVBoxLayout>
#include <QMessageBox>

SpaceApp::SpaceApp(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Ship Control");
    setMinimumSize(400, 300);

    QSlider *thrustSlider = new QSlider(Qt::Horizontal);
    QLCDNumber *speedDisplay = new QLCDNumber();
    QCheckBox *shieldCheck = new QCheckBox("Активировать щиты");
    QPushButton *jumpBtn = new QPushButton("ГИПЕРПРЫЖОК");
    jumpBtn->setEnabled(false);
    
    QRadioButton *stealthMode = new QRadioButton("Тихий ход");
    QRadioButton *battleMode = new QRadioButton("Боевой режим");
    battleMode->setChecked(true);
    
    QDial *navDial = new QDial();
    navDial->setRange(0, 359);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Тяга двигателя:"));
    layout->addWidget(thrustSlider);
    layout->addWidget(speedDisplay);
    layout->addWidget(shieldCheck);
    layout->addWidget(stealthMode);
    layout->addWidget(battleMode);
    layout->addWidget(new QLabel("Навигация:"));
    layout->addWidget(navDial);
    layout->addWidget(jumpBtn);

    connect(thrustSlider, &QSlider::valueChanged, speedDisplay, QOverload<int>::of(&QLCDNumber::display));
    connect(shieldCheck, &QCheckBox::toggled, jumpBtn, &QPushButton::setEnabled);
    connect(jumpBtn, &QPushButton::clicked, [this](){
        QMessageBox::information(this, "Поехали!", "Прыжок совершен успешно!");
    });
    connect(navDial, &QDial::valueChanged, [this](int val){
        this->setWindowTitle(QString("Курс: %1°").arg(val));
    });
}