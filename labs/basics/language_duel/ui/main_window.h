#pragma once

#include <QMainWindow>
#include <QStringList>

#include "core/cat_ai_model.h"
#include "core/models.h"
#include "core/practice_engine.h"

class QLabel;
class QProgressBar;
class QStackedWidget;
class QTimer;
class QAction;

class StartPage;
class TranslationPage;
class GrammarPage;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void chooseDifficulty();
    void startTranslation();
    void startGrammar();
    void startDailyChallenge();
    void submitTranslation();
    void submitGrammar();
    void tick();
    void resetProgress();
    void showAchievements();
    void showLastMistakes();
    void showCatTip();
    void showCatAiModelCard();
    void showSemanticLab();
    void showSoundLab();
    void toggleSound(bool enabled);
    void testSound();

private:
    void buildUi();
    void buildMenu();
    void applyStyle();
    void refreshHeader();
    void loadProgress();
    void saveProgress() const;
    void startSession(ExerciseMode mode);
    void advanceTask();
    void finishSession(bool success, const QString& reason, bool failedByTime = false, bool failedByMistakes = false);
    void failByMistakeLimit();
    void showHelp();
    void playPositiveFeedback();
    void playNegativeFeedback();
    void playAlertFeedback();
    void initializeAudio();
    void playBeepPattern(int count, int intervalMs);
    void setStatus(const QString& message, const QString& tone = "info");
    void setCatMood(const QString& mood, const QString& speech);
    void unlockBadge(const QString& badge);
    QString badgeSummary() const;
    void rememberMistake(const QString& title, const QString& expected);
    QString randomCatCheer() const;
    QString randomCatNudge() const;
    QString randomCatHint() const;
    QString coachSuggestion() const;
    CatAiFeatures currentAiFeatures() const;
    void refreshCatAiPrediction();
    void trainCatAi(bool mistakeHappened);
    void updateDailyProgress();
    int computeReward() const;
    QString currentHelpText() const;

    PracticeEngine engine_;
    SessionData currentSession_;
    int currentIndex_ = 0;
    int remainingSeconds_ = 0;
    int totalScore_ = 0;
    int mistakes_ = 0;
    int streak_ = 0;
    int bestStreak_ = 0;
    int sessionAttempts_ = 0;
    int sessionCorrect_ = 0;
    int advancedAccepts_ = 0;
    int semanticAccepts_ = 0;
    int usedHints_ = 0;
    int completedSessions_ = 0;
    int perfectSessions_ = 0;
    int dailyStreak_ = 0;
    bool dailyChallengeActive_ = false;
    bool sessionActive_ = false;
    bool soundEnabled_ = true;
    double currentMistakeRisk_ = 0.0;
    QString lastDailyDate_;
    QStringList unlockedBadges_;
    QStringList mistakeReview_;
    CatAiModel catAi_;

    QWidget* centralHost_ = nullptr;
    QLabel* scoreLabel_ = nullptr;
    QLabel* modeLabel_ = nullptr;
    QLabel* difficultyLabel_ = nullptr;
    QLabel* timerLabel_ = nullptr;
    QLabel* statusLabel_ = nullptr;
    QLabel* streakLabel_ = nullptr;
    QLabel* bestStreakLabel_ = nullptr;
    QLabel* accuracyLabel_ = nullptr;
    QLabel* levelLabel_ = nullptr;
    QLabel* badgeLabel_ = nullptr;
    QLabel* aiRiskLabel_ = nullptr;
    QLabel* soundLabel_ = nullptr;
    QAction* soundAction_ = nullptr;
    QLabel* catSpeechLabel_ = nullptr;
    QProgressBar* progressBar_ = nullptr;
    QStackedWidget* stack_ = nullptr;
    QTimer* timer_ = nullptr;
    QWidget* catMascot_ = nullptr;

    StartPage* startPage_ = nullptr;
    TranslationPage* translationPage_ = nullptr;
    GrammarPage* grammarPage_ = nullptr;
};
