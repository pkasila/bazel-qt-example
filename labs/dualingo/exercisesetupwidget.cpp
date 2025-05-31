#include "exercisesetupwidget.h"
#include <QSpacerItem>

ExerciseSetupWidget::ExerciseSetupWidget(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    titleLabel = new QLabel("Welcome to EngliQuest!", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    translationButton = new QPushButton("Start Translation Challenge", this);
    translationButton->setMinimumHeight(50);
    QFont buttonFont = translationButton->font();
    buttonFont.setPointSize(12);
    translationButton->setFont(buttonFont);

    grammarButton = new QPushButton("Start Grammar Challenge", this);
    grammarButton->setMinimumHeight(50);
    grammarButton->setFont(buttonFont);


    mainLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));
    mainLayout->addWidget(translationButton);
    mainLayout->addWidget(grammarButton);
    mainLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));


    setLayout(mainLayout);

    connect(translationButton, &QPushButton::clicked, this, &ExerciseSetupWidget::startTranslationChallengeClicked);
    connect(grammarButton, &QPushButton::clicked, this, &ExerciseSetupWidget::startGrammarChallengeClicked);
}
