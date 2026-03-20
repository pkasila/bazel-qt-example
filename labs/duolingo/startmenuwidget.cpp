#include "startmenuwidget.h"
#include <QHBoxLayout>
#include <QSpacerItem>

StartMenuWidget::StartMenuWidget(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 50, 50, 50);

    welcomeLabel = new QLabel("Добро пожаловать в Duolingo!", this);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet("font-size: 24px; margin-bottom: 30px;"); 

    translationButton = new QPushButton("Начать упражнение: Перевод", this);
    grammarButton = new QPushButton("Начать упражнение: Грамматика", this);

    translationButton->setMinimumSize(200, 50);
    grammarButton->setMinimumSize(200, 50);

    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(translationButton, 0, Qt::AlignCenter);
    buttonLayout->addWidget(grammarButton, 0, Qt::AlignCenter);

    mainLayout->addWidget(welcomeLabel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(); 

    connect(translationButton, &QPushButton::clicked, this, &StartMenuWidget::translationExerciseRequested);
    connect(grammarButton, &QPushButton::clicked, this, &StartMenuWidget::grammarExerciseRequested);
}