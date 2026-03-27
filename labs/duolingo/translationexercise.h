#ifndef TRANSLATIONEXERCISE_H
#define TRANSLATIONEXERCISE_H


#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


class TranslationExercise : public QWidget {
    Q_OBJECT


public:
    explicit TranslationExercise(int difficultyLevel, QWidget *parent = nullptr);
    void setDifficulty(int level);
    struct Question {
        QString original;
        QString translation;
    };
    void loadQuestions();


signals:
    void exerciseFinished(); // Сигнал без параметров
    void gameOverSignal();


public slots:
    void checkAnswer();
    void nextQuestion();
    void onTimeout();
    void showHint();
    void startTimer(); // Новый слот для запуска таймера
    void stopTimer();  // Новый слот для остановки таймера


private:
    QLabel* questionLabel;
    QLabel* timerLabel;
    QTextEdit* answerEdit;
    QPushButton* submitButton;
    QPushButton* hintButton;
    QTimer timer;
    int currentQuestion;
    int attempts;
    int correctAnswers;
    int difficultyLevel;
    int totalTimeRemaining; // Single timer for the whole exercise


    QList<Question> questions;


private slots:
    void updateTimer();
    void updateTimerLabel();
};


#endif // TRANSLATIONEXERCISE_H
