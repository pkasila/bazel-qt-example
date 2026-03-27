#ifndef TRANSLATIONEXERCISEWIDGET_H
#define TRANSLATIONEXERCISEWIDGET_H

#include "ExerciseViewBase.h"

class QTextEdit; // Forward declaration

class TranslationExerciseWidget : public ExerciseViewBase {
    Q_OBJECT
public:
    explicit TranslationExerciseWidget(QWidget *parent = nullptr);

protected:
    void setupUi() override;
    void displayCurrentExercise() override;
    void processAnswer() override;
    ExerciseType getExerciseTypeForGenerator() override { return ExerciseType::Translation; }


private:
    QTextEdit *m_answerTextEdit;
};

#endif // TRANSLATIONEXERCISEWIDGET_H