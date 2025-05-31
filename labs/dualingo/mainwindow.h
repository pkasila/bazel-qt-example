#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QMenuBar>
#include <QAction>
#include <QKeyEvent> 

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
    
    QStackedWidget *stackedWidget;
    ExerciseSetupWidget *setupWidget;
    ExerciseRunnerWidget *runnerWidget;

    QProgressBar *taskProgressBar;
    QLabel *scoreLabel;
    QLabel *timerLabel;
    QLabel *mistakesLabel;
    QLabel *difficultyLabel; 

    QAction *difficultyAction;
    QAction *exitAction;
    
    QTimer *exerciseTimer;
    int currentScore;
    int mistakesMade;
    int tasksCompletedInSet; 
    int currentTaskIndex;   

    QString currentDifficulty;
    ExerciseType currentExerciseType;
    QList<TaskData> currentTasks;

    int N_tasksPerSet;
    int M_maxMistakes;
    int T_timeLimitSeconds;

    int m_elapsedSecondsInExercise;

    bool exerciseActive = false;
};

#endif 
