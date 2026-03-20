#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QKeyEvent>

#include "startmenuwidget.h"
#include "translationexercisewidget.h"
#include "grammarexercisewidget.h"
#include "difficultydialog.h"
#include "helpdialog.h"

struct TranslationData {
    QString sentence;
    QString correctAnswer;
    QString helpText;
    QString audioFile;
};

struct GrammarData {
    QString sentenceTemplate;
    QStringList options;
    QString correctAnswer;
    QString helpText;
    QString audioFile;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void showTranslationExercise();
    void showGrammarExercise();
    void handleTranslationSubmit(const QString& userAnswer);
    void handleGrammarSubmit(const QString& userAnswer);
    void updateTimer();
    void timerFinished();
    void changeDifficulty();

private:
    void setupUi();
    void setupMenu();
    void setupData();
    void startExercise(int type);
    void loadNextExercise();
    void endExercise(const QString& message);
    void checkAnswer(const QString& userAnswer);
    QString getCurrentExerciseHelpText() const;
    QString getCurrentExerciseAudioFile() const;

    QStackedWidget *stackedWidget;
    StartMenuWidget *startMenuWidget;
    TranslationExerciseWidget *translationExerciseWidget;
    GrammarExerciseWidget *grammarExerciseWidget;

    QProgressBar *progressBar;
    QLabel *scoreLabel;
    QLabel *timerLabel;

    QTimer *exerciseTimer;
    int timeLeft;
    int totalTimerDuration;

    QList<TranslationData> translationExercises;
    QList<GrammarData> grammarExercises;
    int currentExerciseIndex;
    int totalExercisesInSet;
    int currentScore;
    int incorrectAttempts;
    int maxIncorrectAttempts;

    int currentExerciseType;

    DifficultyDialog::DifficultyLevel currentDifficulty;


    enum PageIndex {
        Page_StartMenu,
        Page_TranslationExercise,
        Page_GrammarExercise
    };
};
#endif // MAINWINDOW_H