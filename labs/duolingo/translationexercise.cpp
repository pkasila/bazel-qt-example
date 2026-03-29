#include "translationexercise.h"

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QVBoxLayout>

TranslationExerciseWidget::TranslationExerciseWidget(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    instructionLabel = new QLabel(tr("Translate the following sentence:"));
    QFont instructionFont = instructionLabel->font();
    instructionFont.setPointSize(instructionFont.pointSize() + 1);
    instructionLabel->setFont(instructionFont);

    sentenceLabel = new QLabel();
    sentenceLabel->setWordWrap(true);
    QFont boldFont = sentenceLabel->font();
    boldFont.setBold(true);
    boldFont.setPointSize(boldFont.pointSize() + 2);
    sentenceLabel->setFont(boldFont);
    sentenceLabel->setStyleSheet("color: #2c3e50;");

    answerEdit = new QLineEdit();
    answerEdit->setStyleSheet(
        "QLineEdit { border: 1px solid #bdc3c7; padding: 5px; border-radius: 3px; }");

    submitButton = new QPushButton(tr("Submit Translation"));
    submitButton->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; border-radius: 5px; padding: 8px; "
        "font-weight: bold; }"
        "QPushButton:hover { background-color: #2980b9; }"
        "QPushButton:disabled { background-color: #aed6f1; }");
    submitButton->setCursor(Qt::PointingHandCursor);

    feedbackLabel = new QLabel();
    feedbackLabel->setWordWrap(true);
    feedbackLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->addWidget(new QLabel(tr("Your translation:")), 0);
    inputLayout->addWidget(answerEdit, 1);

    mainLayout->addWidget(instructionLabel);
    mainLayout->addWidget(sentenceLabel);
    mainLayout->addLayout(inputLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(submitButton, 0, Qt::AlignHCenter);
    mainLayout->addWidget(feedbackLabel);
    mainLayout->addStretch();

    connect(submitButton, &QPushButton::clicked, this, &TranslationExerciseWidget::submitClicked);
    connect(answerEdit, &QLineEdit::returnPressed, this, &TranslationExerciseWidget::submitClicked);

    setLayout(mainLayout);
}

void TranslationExerciseWidget::setTask(const TranslationTask& task) {
    sentenceLabel->setText(task.sourceSentence);
    currentCorrectAnswer = task.correctAnswer;
    answerEdit->clear();
    feedbackLabel->clear();
    answerEdit->setFocus();
}

QString TranslationExerciseWidget::getUserAnswer() const {
    return answerEdit->text().trimmed();
}

void TranslationExerciseWidget::clearAnswer() {
    answerEdit->clear();
}

void TranslationExerciseWidget::showCorrectAnswer(const QString& correctAnswer) {
    feedbackLabel->setText(tr("The correct answer was: %1").arg(correctAnswer));
    feedbackLabel->setStyleSheet("color: blue;");
}