#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QTextEdit>
#include <QRadioButton>
#include <QSoundEffect>
#include <QButtonGroup>
#include "Tasks.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void startTranslation();
    void startGrammar();
    void submitTranslation();
    void submitGrammar();
    void updateTimer();
    void openSettings();
    void returnToMenu();

private:
    void setupUI();
    void applyTheme();
    void initGame(int type); // 1 = trans, 2 = grammar
    void loadNextTask();
    void processAnswer(bool isCorrect);
    void endGame(bool win, const QString &reason = QString());
    void updateHUD();
    void showHelpMemo();

    // UI Elements
    QStackedWidget *stackedWidget;
    QProgressBar *progressBar;
    QLabel *scoreLabel;
    QLabel *livesLabel;
    QLabel *timeLabel;
    QLabel *taskPromptLabel;
    QLabel *grammarText1;
    QLabel *grammarText2;

    QTextEdit *translationInput;
    QRadioButton *radioBtn1;
    QRadioButton *radioBtn2;
    QRadioButton *radioBtn3;
    QButtonGroup *radioGroup;

    // Game Logic
    QTimer *timer;
    int score;
    int maxLives;
    int currentLives;
    int maxTime;
    int currentTime;
    int currentExerciseType;
    int taskIndex;
    int currentMistakes;
    int maxMistakes;
    int currentTaskCount;
    QString currentHint;
    std::vector<TranslationTask> tTasks;
    std::vector<GrammarTask> gTasks;

    QLabel *mistakesLabel;

    // Media
    QSoundEffect *sndCorrect;
    QSoundEffect *sndWrong;
};
