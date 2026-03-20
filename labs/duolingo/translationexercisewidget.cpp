#include "translationexercisewidget.h"
#include <QHBoxLayout>
#include "mainwindow.h"

TranslationExerciseWidget::TranslationExerciseWidget(QWidget *parent) : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    instructionLabel = new QLabel("Переведите следующее предложение:", this);
    instructionLabel->setStyleSheet("font-size: 18px; margin-bottom: 10px;");

    sentenceLabel = new QLabel(this);
    sentenceLabel->setWordWrap(true);
    sentenceLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 20px;");

    answerInput = new QLineEdit(this); 
    answerInput->setPlaceholderText("Введите ваш перевод здесь");
    answerInput->setStyleSheet("padding: 8px; font-size: 16px;");

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
    mainLayout->addWidget(answerInput);
    mainLayout->addWidget(submitButton, 0, Qt::AlignCenter);
    mainLayout->addWidget(feedbackLabel);
    mainLayout->addStretch(); 

    connect(submitButton, &QPushButton::clicked, this, [this](){
        emit answerSubmitted(answerInput->text()); 
    });

}

void TranslationExerciseWidget::setExerciseData(const TranslationData& data)
{
    sentenceLabel->setText(data.sentence);
    answerInput->clear(); 
    feedbackLabel->hide(); 
    if (!data.audioFile.isEmpty()) {
        audioButton->show();
    } else {
        audioButton->hide();
    }
}

void TranslationExerciseWidget::showFeedback(const QString& message, bool isCorrect)
{
    feedbackLabel->setText(message);
    feedbackLabel->setStyleSheet(QString("font-weight: bold; margin-top: 10px; color: %1;")
                                 .arg(isCorrect ? "green" : "red"));
    feedbackLabel->show();
}