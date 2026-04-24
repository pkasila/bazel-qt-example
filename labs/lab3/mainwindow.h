#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAudioOutput> 
#include <QMediaPlayer>
#include <QMainWindow>
#include <QSoundEffect>
#include <QStackedWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QKeyEvent>
#include "exercise_data.h"

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void setupUI();
    void setupMenu();
    void initData();
    void resetSession();
    void nextTask();
    void finishSession(bool success, QString message);
    
    void showDifficultyDialog();
    void startTranslationMode();
    void startGrammarMode();
    void submitAnswer();
    void updateTimer();
    void backToMenu();
    void showHint();
    int calculateLevenshteinDistance(const QString &s1, const QString &s2);

    QStackedWidget *stackedWidget;
    QWidget *menuPage;
    QWidget *exercisePage;
    QLabel *scoreLabel;
    QLabel *heartsLabel;
    QLabel *timerLabel;
    QLabel *taskLabel;
    QProgressBar *progressBar;
    QTextEdit *translationEdit;
    QWidget *grammarContainer;
    QButtonGroup *grammarGroup;
    QTimer *sessionTimer;
    QMediaPlayer *successPlayer;
    QAudioOutput *successOutput;
    QMediaPlayer *failurePlayer;
    QAudioOutput *failureOutput;
    
    int timeLeft;
    int score = 0;
    int hearts = 3;
    int currentTaskIdx = 0;
    int totalTasks = 5;
    bool isTranslationMode = true;
    bool timerEnabled = true;
    Difficulty currentDifficulty = Difficulty::Easy;
    QVector<TranslationTask> translationTasks;
    QVector<GrammarTask> grammarTasks;
};

#endif