#pragma once

#include "audiohelper.h"
#include "question.h"

#include <QButtonGroup>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>

class QEvent;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void startTranslation();
    void startGrammar();
    void submitAnswer();
    void tickTimer();
    void changeDifficulty();
    void showHint();
    void showAudioStatus();
    void returnToWelcome();

private:
    void createMenuBar();
    void createInterface();
    void loadExercise(ExerciseType type);
    void showNextQuestion();
    void clearGrammarOptions();
    void setGrammarOptions(const QStringList& options);
    void finishExercise(bool success, const QString& message);
    void updateSidebar();
    void setFeedback(const QString& text, bool success);
    int maxAttemptsForDifficulty() const;
    QString difficultyName() const;

    int score = 0;
    int difficulty = 0;
    int remainingAttempts = 3;
    int currentQuestionIndex = 0;
    int timeLeft = 0;
    ExerciseType currentType = ExerciseType::Translation;
    QList<Question> currentQuestions;

    QTimer* timer = nullptr;
    AudioPlayer* audioPlayer = nullptr;

    QFrame* sidebar = nullptr;
    QLabel* scoreLabel = nullptr;
    QLabel* difficultyLabel = nullptr;
    QLabel* soundLabel = nullptr;

    QStackedWidget* contentStack = nullptr;
    QWidget* welcomePage = nullptr;
    QWidget* exercisePage = nullptr;

    QLabel* exerciseTitleLabel = nullptr;
    QLabel* timerLabel = nullptr;
    QLabel* attemptsLabel = nullptr;
    QLabel* statusLabel = nullptr;
    QLabel* feedbackLabel = nullptr;
    QLabel* questionLabel = nullptr;
    QProgressBar* progressBar = nullptr;

    QStackedWidget* inputStack = nullptr;
    QLineEdit* translationInput = nullptr;
    QWidget* grammarWidget = nullptr;
    QVBoxLayout* grammarLayout = nullptr;
    QButtonGroup* grammarGroup = nullptr;

    QPushButton* submitButton = nullptr;
};
