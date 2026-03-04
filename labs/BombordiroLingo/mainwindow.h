#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include "exercisetranslation.h"
#include "exercisegrammar.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTranslationClicked();
    void onGrammarClicked();
    void onSubmitClicked();
    void updateTimer();
    void togglePause();

private:
    void setupUI();
    void setupMenu();
    void startExerciseTimer();
    void stopExerciseTimer();
    void resetExercise();
    void updateScoreDisplay();

    QStackedWidget *stackedWidget;
    ExerciseTranslation *translationExercise;
    ExerciseGrammar *grammarExercise;
    QPushButton *translationBtn;
    QPushButton *grammarBtn;
    QPushButton *submitBtn;
    QProgressBar *progressBar;
    QLabel *scoreLabel;
    QLabel *timerLabel;
    QTimer *exerciseTimer;
    int timeLeft;
    int totalScore;
    QString currentDifficulty;
    bool isPaused;
    QPushButton *pauseButton;
    QHBoxLayout *buttonLayout;


protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // MAINWINDOW_H
