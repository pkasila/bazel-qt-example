#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QMenuBar>
#include <QAction>
#include <QKeyEvent> // For H key

#include "taskdata.h"
#include "exercisesetupwidget.h"
#include "exerciserunnerwidget.h"
#include "difficultydialog.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void showDifficultyDialog();
    void handleDifficultyChanged(const QString& newDifficulty);

    void startTranslationExercise();
    void startGrammarExercise();

    void handleSubmitAnswer();
    void updateTimerDisplay();
    void handleExerciseTimeout();

    void advanceToNextTaskOrEnd();


private:
    void createMenus();
    void createStatusBar();
    void setupCentralWidget();

    void loadTasksForCurrentExercise();
    void beginExercise();
    void displayCurrentTask();
    void processAnswer(bool isCorrect);
    void endExercise(const QString& reasonMessage, bool awardedPoints);
    void resetToSetupScreen();
    void updateStatusDisplay();


    // UI Elements
    QStackedWidget *stackedWidget;
    ExerciseSetupWidget *setupWidget;
    ExerciseRunnerWidget *runnerWidget;

    QProgressBar *taskProgressBar;
    QLabel *scoreLabel;
    QLabel *timerLabel;
    QLabel *mistakesLabel;
    QLabel *difficultyLabel; // Display current difficulty

    // Actions
    QAction *difficultyAction;
    QAction *exitAction;

    // Exercise Logic
    QTimer *exerciseTimer;
    int currentScore;
    int mistakesMade;
    int tasksCompletedInSet; // How many tasks answered in current set
    int currentTaskIndex;   // Index in currentTasks list

    QString currentDifficulty;
    ExerciseType currentExerciseType;
    QList<TaskData> currentTasks;

    // Config based on difficulty
    int N_tasksPerSet;
    int M_maxMistakes;
    int T_timeLimitSeconds;

    int m_elapsedSecondsInExercise;

    bool exerciseActive = false;
};

#endif // MAINWINDOW_H
