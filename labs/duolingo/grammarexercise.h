#ifndef GRAMMAREXERCISE_H
#define GRAMMAREXERCISE_H

// NOLINTBEGIN

#include <QWidget>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>
#include <QGroupBox>
#include "exercisedata.h"

class GrammarExerciseWidget : public QWidget {
    Q_OBJECT

public:
    explicit GrammarExerciseWidget(QWidget *parent = nullptr);
    void setTask(const GrammarTask& task);
    int getSelectedOption() const; 
    void showCorrectAnswer(int correctIndex);

signals:
    void submitClicked();
    void helpRequested();

private:
    QLabel *instructionLabel;
    QLabel *questionLabel;
    QGroupBox *optionsGroup;
    QVBoxLayout *optionsLayout;
    QVector<QRadioButton*> radioButtons;
    QPushButton *submitButton;
    QLabel *feedbackLabel;

    int currentCorrectOptionIndex;
};
// NOLINTEND

#endif // GRAMMAREXERCISE_H