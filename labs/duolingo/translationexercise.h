#ifndef TRANSLATIONEXERCISE_H
#define TRANSLATIONEXERCISE_H

#include "exercisedata.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class TranslationExerciseWidget : public QWidget {
    Q_OBJECT

   public:
    explicit TranslationExerciseWidget(QWidget* parent = nullptr);
    void setTask(const TranslationTask& task);
    [[nodiscard]] QString getUserAnswer() const;
    void clearAnswer();
    void showCorrectAnswer(const QString& correctAnswer);

   signals:
    void submitClicked();
    void helpRequested();

   private:
    QLabel* instructionLabel;
    QLabel* sentenceLabel;
    QLineEdit* answerEdit;
    QPushButton* submitButton;
    QLabel* feedbackLabel;

    QString currentCorrectAnswer;
};

#endif