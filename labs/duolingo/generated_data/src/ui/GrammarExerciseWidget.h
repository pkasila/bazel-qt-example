#ifndef GRAMMAREXERCISEWIDGET_H
#define GRAMMAREXERCISEWIDGET_H

#include "ExerciseViewBase.h"

class QGroupBox;
class QVBoxLayout;
class QRadioButton;

class GrammarExerciseWidget : public ExerciseViewBase {
    Q_OBJECT
public:
    explicit GrammarExerciseWidget(QWidget *parent = nullptr);

protected:
    void setupUi() override;
    void displayCurrentExercise() override;
    void processAnswer() override;
    ExerciseType getExerciseTypeForGenerator() override { return ExerciseType::Grammar; }

private:
    void clearRadioButtons();

    QGroupBox *m_optionsGroup;
    QVBoxLayout *m_radioButtonsLayout;
    QList<QRadioButton*> m_radioButtons;
};

#endif // GRAMMAREXERCISEWIDGET_H