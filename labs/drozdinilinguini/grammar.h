#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <vector>
#include <QButtonGroup>
#include <QDateTime>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QTime>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QRandomGenerator>
#include <QPropertyAnimation>
#include <QAbstractAnimation>

class GrammarMode : public QWidget {
    Q_OBJECT

public:
    struct GrammarTask {
        int id;
        QString question;
        int correct_id;
        QString option1;
        QString option2;
        QString option3;
        QString option4;
        int difficulty;
    };

    explicit GrammarMode(QWidget* parent = nullptr);
    void loadTask(int mode);
    void setMode(int mode);
    void startSession();
    static void loadTasksFromMemory();

signals:
    void answerSubmitted(int answerId, bool isCorrect);
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
    };

    void setupUI();
    void clearOptions();
    void endSession(bool success);
    void showFeedback(bool isCorrect);
    int calculateScore();
    void updateTimerDisplay();

    static std::vector<GrammarTask> tasks;
    QVBoxLayout* layout;
    QLabel* instructionLabel;
    QLabel* questionLabel;
    QVBoxLayout* optionsLayout;
    QButtonGroup* optionsGroup;
    QPushButton* submitButton;
    QPushButton* startButton;
    QProgressBar* progressBar;
    QTimer* timer;
    QLabel* timerLabel;

    int correctAnswerId;
    int currentMode = 1;
    SessionState currentSession;
    QTime remainingTime;

    const int totalTasks = 20;
    const int maxAttempts = 3;
};

#endif