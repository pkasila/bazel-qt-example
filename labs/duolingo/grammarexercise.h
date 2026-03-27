#ifndef GRAMMAREXERCISE_H
#define GRAMMAREXERCISE_H


#include <QWidget>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QButtonGroup>


class GrammarExercise : public QWidget {
    Q_OBJECT


public:
    explicit GrammarExercise(int difficultyLevel, QWidget *parent = nullptr);
    void setDifficulty(int level);
    struct Question {
        QString sentence;
        QString correctOption;
        QStringList options;
    };
    void loadQuestions();


signals:
    void exerciseFinished();
    void gameOverSignal();


public slots:
    void checkAnswer();
    void nextQuestion();
    void onTimeout();
    void showHint();
    void startTimer();
    void stopTimer();


protected:
    void resizeEvent(QResizeEvent *event) override;


private:
    QLabel* questionLabel;
    QLabel* timerLabel;
    QRadioButton* optionA;
    QRadioButton* optionB;
    QRadioButton* optionC;
    QPushButton* submitButton;
    QPushButton* hintButton;
    QTimer timer;


    int currentQuestion;
    int attempts;
    int correctAnswers;
    int difficultyLevel;
    int totalTimeRemaining;
    int baseFontSize;


    QList<Question> questions;


private slots:
    void updateTimer();
    void updateTimerLabel();


private:
    void adjustFontsAndSizes(const QSize& newSize);
};


#endif // GRAMMAREXERCISE_H
