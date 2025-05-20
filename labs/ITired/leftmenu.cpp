#include "leftmenu.h"

LeftMenu::LeftMenu(QWidget *parent)
    : QWidget{parent}
{

    QVBoxLayout* layout = new QVBoxLayout(this);
    translate = new QPushButton("Translate");
    grammar = new QPushButton("Grammar");
    globalScoreL = new QLabel("You have 0 points");
    layout->addWidget(globalScoreL);
    layout->addWidget(translate);
    layout->addWidget(grammar);
    connections();
    click = new QSoundEffect();
    click->setSource(QUrl("qrc:/audio/click.wav"));
}

void LeftMenu::updateLabel(int num) {
    globalScoreL->setText(QString("You have %1 points").arg(num));
}

void LeftMenu::connections() {
    connect(translate, &QPushButton::clicked, this, &LeftMenu::trButClicked);
    connect(translate, &QPushButton::clicked, this, [&]() {
        click->play();
    });
    connect(grammar, &QPushButton::clicked, this, &LeftMenu::grButClicked);
    connect(grammar, &QPushButton::clicked, this, [&]() {
        click->play();
    });
}
