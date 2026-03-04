#ifndef EXERCISEGRAMMAR_H
#define EXERCISEGRAMMAR_H

#include <QWidget>
#include <QStringList>
#include <QList>
#include <QTextEdit>

class QLabel;
class QRadioButton;
class QButtonGroup;
class QVBoxLayout;

class ExerciseGrammar : public QWidget
{
    Q_OBJECT

public:
    explicit ExerciseGrammar(QWidget *parent = nullptr);
    void startExercise(const QString &difficulty);
    bool checkAnswer();

private:
    void generateQuestions(const QString &difficulty);
    void showNextQuestion();

    QVBoxLayout *layout;
    QLabel *questionLabel;
    QButtonGroup *optionsGroup;
    QList<QRadioButton*> optionButtons;
    QStringList questions;
    QList<int> correctAnswers;
    int currentQuestion;
    int attemptsLeft;

};

#endif // EXERCISEGRAMMAR_H
