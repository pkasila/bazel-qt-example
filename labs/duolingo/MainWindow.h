#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Exercises.h"
#include "TaskManager.h"

#include <QtCore/QString>
#include <QtCore/QTimer>
#ifndef __BAZEL_SCANNER__
#include <QtTextToSpeech/QTextToSpeech>
#endif
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QStackedWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT
   public:
    MainWindow(QWidget* parent = nullptr);

   protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

   private slots:
    void startTranslation();
    void startGrammar();
    void startMath();
    void changeDifficulty();
    void onSubmitTranslation();
    void onSubmitGrammar();
    void onSubmitMath();
    void updateTimer();
    void showHint();
    void giveUp();
    void submitCurrentTask();
    void hideNotification();

   private:
    void setupUi();
    void nextTask();
    void generateMathTask();
    void endExercise(bool success);
    void failExercise(const QString& reason);
    void applyStyle();
    void showNotification(const QString& text, bool isError);
    void updateLivesDisplay();

    QStackedWidget* stack;
    QWidget* menuWidget;
    TranslationWidget* transWidget;
    GrammarWidget* grammarWidget;
    MathWidget* mathWidget;

    TaskManager manager;
    QProgressBar* progress;
    QLabel* scoreLabel;
    QTimer* timer;
    QTextToSpeech* tts;

    enum ExType { None, Translation, Grammar, Math } currentType = None;

    enum DifficultyLevel { Easy, Medium, Hard, UltraHard } currentDifficulty = Medium;

    int score = 0;
    int mistakesCount = 0;
    int currentTaskIdx = 0;
    int tasksDoneInSession = 0;

    int tasksPerSession = 10;
    int maxMistakes = 3;
    int timeLimit = 30;  // in seconds
    int timeLeft = 0;
    DifficultyLevel currentMathDifficulty = Medium;
    int currentMathAnswer = 0;

    QString currentHint;

    QWidget* topBar;
    QLabel* timerLabel;
    QLabel* livesLabel;
    QPushButton* hintBtn;
    QPushButton* giveUpBtn;

    QLabel* notificationLabel;
    QTimer* notificationTimer;
};
#endif  // MAINWINDOW_H