#include "translationexercisewidget.h"
#include "exercisestructures.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QSizePolicy>
#include <QDebug>

TranslationExerciseWidget::TranslationExerciseWidget(QWidget *parent) : BaseExerciseWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    instructionLabel = new QLabel(tr("Translate the following text:"));
    instructionLabel->setStyleSheet("font-weight: bold;");
    layout->addWidget(instructionLabel);

    sentenceToTranslateLabel = new QLabel(tr("Sentence will appear here."));
    sentenceToTranslateLabel->setWordWrap(true);
    sentenceToTranslateLabel->setStyleSheet("font-size: 16px; border: 1px solid lightgray; padding: 5px;");
    layout->addWidget(sentenceToTranslateLabel);

    answerTextEdit = new QTextEdit();
    answerTextEdit->setPlaceholderText(tr("Enter your translation here..."));
    answerTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    layout->addWidget(answerTextEdit);

    submitButton = new QPushButton(tr("Submit Translation"));
    submitButton->setMinimumHeight(40);
    submitButton->setStyleSheet("padding: 10px 20px; font-size: 14px;");
    layout->addWidget(submitButton, 0, Qt::AlignRight);

    setLayout(layout);
}

void TranslationExerciseWidget::loadTask(const QVariant& taskData) {
    currentLoadedTaskData = taskData;
    if (taskData.canConvert<TranslationTask>()) {
        TranslationTask task = taskData.value<TranslationTask>();
        sentenceToTranslateLabel->setText(task.textToTranslate);
        clearFields();
        answerTextEdit->setFocus();
    } else {
        qWarning() << "TranslationExerciseWidget: Failed to convert taskData to TranslationTask";
        sentenceToTranslateLabel->setText(tr("Error loading task."));
    }
}

QString TranslationExerciseWidget::getUserAnswer() const {
    return answerTextEdit->toPlainText().trimmed();
}

void TranslationExerciseWidget::clearFields() {
    answerTextEdit->clear();
}

QPushButton* TranslationExerciseWidget::getSubmitButton() const {
    return submitButton;
}
