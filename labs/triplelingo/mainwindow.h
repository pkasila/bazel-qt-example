#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVariant>
#include "exercisestructures.h"
#include "exercisedata.h"

class QLabel;
class QPushButton;
class QStackedWidget;
class QProgressBar;
class QTimer;
class TranslationExerciseWidget;
class GrammarExerciseWidget;
class BaseExerciseWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void openDifficultyDialog();
    void startExercise(ExerciseType type);
    void handleSubmitAnswer();
    void updateTimerAndCheck();
    void showCurrentHelp();
    void cancelCurrentExercise();

private:
    void setupUi();
    void setupMenu();
    void setupConnections();

    void loadNextTask();
    void completeExercise(bool success, const QString& reason = "");
    void updateScoreDisplay();
    void updateTimerDisplay();
    void setExerciseButtonsEnabled(bool enabled);
    void showWelcomeMessage();
    void selectTasksForCurrentExercise(ExerciseType type);

    QLabel *scoreLabel;
    QLabel *timerLabel;
    QProgressBar *progressBar;
    QStackedWidget *exerciseStackWidget;
    QPushButton *translationButton;
    QPushButton *grammarButton;
    QWidget *welcomeWidget;
    QPushButton *cancelExerciseButton;

    TranslationExerciseWidget *translationExerciseWidget;
    GrammarExerciseWidget *grammarExerciseWidget;
    BaseExerciseWidget *currentExerciseWidgetPtr;

    ExerciseData exerciseDataHandler;
    QString currentDifficulty;
    int currentScore;
    ExerciseType activeExerciseType;
    QList<QVariant> currentTasksList;
    int currentTaskIndex;
    int incorrectAttempts;
    int timeLeftSeconds;

    QTimer *exerciseTimer;
};
#endif // MAINWINDOW_H
