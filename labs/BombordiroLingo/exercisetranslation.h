#ifndef EXERCISETRANSLATION_H
#define EXERCISETRANSLATION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>

class ExerciseTranslation : public QWidget
{
    Q_OBJECT

public:
    explicit ExerciseTranslation(QWidget *parent = nullptr);
    void startExercise(const QString &difficulty);
    bool checkAnswer();

private:
    void generateQuestions(const QString &difficulty);
    void showNextQuestion();

    QVBoxLayout *layout;
    QLabel *questionLabel;
    QTextEdit *answerEdit;
    QStringList questions;
    QStringList answers;
    int currentQuestion;
    int attemptsLeft;

};

#endif // EXERCISETRANSLATION_H
