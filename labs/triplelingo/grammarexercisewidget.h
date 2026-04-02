#ifndef GRAMMAREXERCISEWIDGET_H
#define GRAMMAREXERCISEWIDGET_H

#include "baseexercisewidget.h"
#include <QList>

class QLabel;
class QRadioButton;
class QButtonGroup;
class QPushButton;
class QVBoxLayout;

class GrammarExerciseWidget : public BaseExerciseWidget
{
    Q_OBJECT

public:
    explicit GrammarExerciseWidget(QWidget *parent = nullptr);
    ~GrammarExerciseWidget() override;

    void loadTask(const QVariant& taskData) override;
    QString getUserAnswer() const override;
    void clearFields() override;

    QPushButton* getSubmitButton() const;

private:
    QLabel *instructionLabel;
    QLabel *sentenceLabel;
    QWidget *optionsContainer;
    QVBoxLayout *optionsLayout;
    QButtonGroup *buttonGroup;
    QList<QRadioButton*> radioButtons;
    QPushButton *submitButton;
};

#endif // GRAMMAREXERCISEWIDGET_H
