#ifndef TRANSLATIONEXERCISEWIDGET_H
#define TRANSLATIONEXERCISEWIDGET_H

#include "baseexercisewidget.h"

class QLabel;
class QTextEdit;
class QPushButton;

class TranslationExerciseWidget : public BaseExerciseWidget
{
    Q_OBJECT

public:
    explicit TranslationExerciseWidget(QWidget *parent = nullptr);

    void loadTask(const QVariant& taskData) override;
    QString getUserAnswer() const override;
    void clearFields() override;

    QPushButton* getSubmitButton() const;

private:
    QLabel *instructionLabel;
    QLabel *sentenceToTranslateLabel;
    QTextEdit *answerTextEdit;
    QPushButton *submitButton;
};

#endif // TRANSLATIONEXERCISEWIDGET_H
