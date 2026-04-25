#pragma once

#include <QObject>
#include <QTimer>
#include <vector>
#include <QStringList>
#include "question.h"

struct CustomGameConfig {
    int timeLimit = 45;
    int attempts = 5;
    int questionsCount = 7;
};

class ExerciseController : public QObject {
    Q_OBJECT
public:
    explicit ExerciseController(QObject *parent = nullptr);
    
    void startExercise(int difficulty, int numQuestions, const CustomGameConfig& customConf = CustomGameConfig(), int filterType = 0);
    void submitAnswer(const QString& answer);
    void skipQuestion();
    void stop();

    Question getCurrentQuestion() const;
    int getCurrentScore() const { return score; }
    int getProgress() const { return currentIndex; }
    int getTotalQuestions() const { return questions.size(); }
    int getAttemptsLeft() const { return maxAttempts - currentAttempts; }
    int getTimeLeft() const { return timeLeft; }
    int getCurrentStreak() const { return currentStreak; }
    float getScoreMultiplier() const { return scoreMultiplier; }

signals:
    void questionStarted(const Question& question);
    void correctAnswer();
    void wrongAnswer(const QString& correctAnswerSample);
    void questionSkipped();
    void exerciseFinished(bool won, int finalScore);
    void timeUpdated(int secondsLeft);
    void streakUpdated(int streak, float multiplier);

private slots:
    void onTick();

private:
    void endExercise(bool won);
    void nextQuestion();

    std::vector<Question> questions;
    QTimer *timer;
    int currentIndex;
    int score;
    int maxAttempts;
    int currentAttempts;
    int timeLeft;
    
    int currentDifficulty; // 1, 2, 3, or 4 (Custom)
    int currentStreak;
    float scoreMultiplier;
    CustomGameConfig cachedCustomParams;
};
