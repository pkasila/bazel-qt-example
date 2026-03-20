#include "grammarexercisewidget.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include "mainwindow.h"

GrammarExerciseWidget::GrammarExerciseWidget(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    instructionLabel = new QLabel("Выберите правильный вариант:", this);
    instructionLabel->setStyleSheet("font-size: 18px; margin-bottom: 10px;");

    sentenceLabel = new QLabel(this);
    sentenceLabel->setWordWrap(true);
    sentenceLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 20px;");

    optionsGroupBox = new QGroupBox("Варианты", this);
    optionsLayout = new QVBoxLayout(optionsGroupBox); 
    optionsButtonGroup = new QButtonGroup(this);

    submitButton = new QPushButton("Отправить", this);
    submitButton->setMinimumSize(150, 40);
    submitButton->setStyleSheet("font-size: 16px;");

    feedbackLabel = new QLabel(this);
    feedbackLabel->setAlignment(Qt::AlignCenter);
    feedbackLabel->setStyleSheet("font-weight: bold; margin-top: 10px;");
    feedbackLabel->hide();

    audioButton = new QPushButton("Слушать", this);
    audioButton->setIcon(QIcon::fromTheme("media-playback-start"));
    audioButton->setFlat(true);
    audioButton->hide();

    QHBoxLayout *sentenceLayout = new QHBoxLayout();
    sentenceLayout->addWidget(sentenceLabel);
    sentenceLayout->addWidget(audioButton);


    mainLayout->addWidget(instructionLabel);
    mainLayout->addLayout(sentenceLayout);
    mainLayout->addWidget(optionsGroupBox);
    mainLayout->addWidget(submitButton, 0, Qt::AlignCenter);
    mainLayout->addWidget(feedbackLabel);
    mainLayout->addStretch();

    connect(submitButton, &QPushButton::clicked, this, &GrammarExerciseWidget::onSubmitClicked);
}

void GrammarExerciseWidget::setExerciseData(const GrammarData& data)
{
    sentenceLabel->setText(data.sentenceTemplate);

    QList<QAbstractButton *> buttons = optionsButtonGroup->buttons();
    for (QAbstractButton *button : buttons) {
        optionsButtonGroup->removeButton(button);
        optionsLayout->removeWidget(button);
        delete button;
    }

    for (const QString& option : data.options) {
        QRadioButton *radioButton = new QRadioButton(option, optionsGroupBox);
        optionsLayout->addWidget(radioButton);
        optionsButtonGroup->addButton(radioButton);
    }

    if (!buttons.isEmpty()) {
        optionsButtonGroup->setExclusive(false);
        buttons.first()->setChecked(false);
        optionsButtonGroup->setExclusive(true);
    }


    feedbackLabel->hide();
    if (!data.audioFile.isEmpty()) {
        audioButton->show();
    } else {
        audioButton->hide();
    }
}

void GrammarExerciseWidget::showFeedback(const QString& message, bool isCorrect)
{
    feedbackLabel->setText(message);
    feedbackLabel->setStyleSheet(QString("font-weight: bold; margin-top: 10px; color: %1;")
                                 .arg(isCorrect ? "green" : "red"));
    feedbackLabel->show();
}

void GrammarExerciseWidget::onSubmitClicked()
{
    QAbstractButton *checkedButton = optionsButtonGroup->checkedButton();
    if (checkedButton) {
        emit answerSubmitted(checkedButton->text());
    } else {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите один из вариантов.");
    }
}