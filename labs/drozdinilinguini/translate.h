#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <vector>
#include <QDateTime>
#include <QKeyEvent>
#include <QLabel>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QRandomGenerator>
#include <QPropertyAnimation>  // Добавлено
#include <QAbstractAnimation>

class TranslateMode : public QWidget {
    Q_OBJECT

public:
    struct TranslateTask {
        int id;
        QString english;
        QString german;
        int difficulty;
    };

    explicit TranslateMode(QWidget* parent = nullptr);
    void loadTask();
    void setMode(int mode);
    void startSession();
    static void loadTasksFromMemory();

signals:
    void answerSubmitted(bool isCorrect);
    void scoreUpdated(int newScore);
    void sessionCompleted(bool success);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void checkAnswer();

private:
    struct SessionState {
        int score = 0;
        int correctAnswers = 0;
        int incorrectAttempts = 0;
        bool isActive = false;
        QDateTime startTime;
        int lastTaskId = 0;
    };

    void setupUI();
    void endSession(bool success);
    void showFeedback(bool isCorrect);
    int calculateScore();
    void updateTimerDisplay();

    static std::vector<TranslateTask> tasks;
    QVBoxLayout* layout;
    QLabel* instructionLabel;
    QLabel* sentenceLabel;
    QTextEdit* answerInput;
    QPushButton* submitButton;
    QPushButton* startButton;
    QProgressBar* progressBar;
    QTimer* timer;
    QLabel* timerLabel;

    QString correctAnswer;
    int currentMode = 1;
    SessionState currentSession;
    QTime remainingTime;

    const int totalTasks = 20;
    const int maxAttempts = 3;
};

#endif