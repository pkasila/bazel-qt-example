#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "exercisedata.h"
#include "grammarexercise.h"
#include "multicorrectgrammarexercise.h"
#include "translationexercise.h"

#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QShortcut>
#include <QSoundEffect>
#include <QStackedWidget>
#include <QTimer>
#include <QVector>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPushButton;
QT_END_NAMESPACE

const int TOTAL_TASKS_PER_EXERCISE = 3;
const int MAX_MISTAKES_ALLOWED = 2;
const int EXERCISE_TIME_SECONDS = 120;

enum class ExerciseType { None, Translation, GrammarSingle, GrammarMulti };

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   protected:
    void showEvent(QShowEvent* event) override;

   private slots:
    void openSettingsDialog();
    void startTranslationExercise();
    void startGrammarSingleExercise();
    void startGrammarMultiExercise();
    void handleTranslationSubmit();
    void handleGrammarSingleSubmit();
    void handleGrammarMultiSubmit();
    void updateTimer();
    void exerciseTimeUp();
    void stopCurrentExercise();
    void onMeditateButtonClicked();

   private:
    void createMenus();
    void createMainLayout();
    void setupNewExerciseSet(ExerciseType type);
    void presentNextTask();
    void concludeExerciseSession(bool wasSuccessful);
    void updatePlayerScoreDisplay();
    void resetApplicationToIdleState();
    void showHelp();

    static int calculateLevenshteinDistance(const QString& s1, const QString& s2);
    const double LEVENSHTEIN_SIMILARITY_THRESHOLD = 0.20;

    void applyFeedbackStyleToLabel(QLabel* label, bool isCorrect);

    void initializeSounds();
    void playSoundEffect(QSoundEffect* sound, bool shouldLoop = false);

    QSoundEffect* correctAnswerSound;
    QSoundEffect* incorrectAnswerSound;
    QSoundEffect* exerciseStartCue;
    QSoundEffect* timerExpiredSound;
    QSoundEffect* applicationStartSound;
    QSoundEffect* meditationAudio;

    QMenu* settingsMenu;
    QAction* changeDifficultyAction;

    QWidget* centralWidgetContainer;
    QStackedWidget* exerciseDisplayArea;
    QLabel* welcomeMessageLabel;
    TranslationExerciseWidget* translationExerciseWidget;
    GrammarExerciseWidget* grammarSingleChoiceExerciseWidget;
    MultiCorrectGrammarExerciseWidget* grammarMultiChoiceExerciseWidget;

    QPushButton* translationModeButton;
    QPushButton* grammarSingleModeButton;
    QPushButton* grammarMultiModeButton;
    QPushButton* stopCurrentExerciseButton;
    QPushButton* meditationToggleButton;
    QProgressBar* taskProgressBar;
    QLabel* currentScoreLabel;
    QLabel* remainingTimeLabel;
    QLabel* userNotificationLabel;
    
    QShortcut* helpShortcut;

    Difficulty currentDifficultyLevel;
    ExerciseType activeExerciseType;
    int tasksCompletedInSet;
    int mistakesMadeInSet;
    int totalPlayerScore;
    QTimer* exerciseSessionTimer;
    int secondsRemainingInSession;

    QVector<TranslationTask> currentTranslationTaskSet;
    QVector<GrammarTask> currentGrammarSingleTaskSet;
    QVector<MultiCorrectGrammarTask> currentGrammarMultiTaskSet;
    int currentTaskIndexInSet;
};

#endif