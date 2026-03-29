#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QStackedWidget>

#include "grammar.h"
#include "translation.h"

struct Question {
    QString question;
    QStringList options;
    int correctOption;
};

struct Sentence {
    QString original;
    QString translation;
    int level;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void ShowGrammar();
    void ShowTranslation();
    void ShowWelcome();
    void OnDifficultyAction(const QAction *action);
    void AddGrammarQ();
    void AddTranslationS();
    void ShowHelp();
    void ShowHelpExercise();
    void OnExerciseDone(int score);

private:
    void setupMenu();
    void setupUi();
    void loadData();
    void updateTitle();
    QTimer* StartAndStopTimer() const;

    QMenu *exerciseMenu;
    QMenu *editMenu;
    QMenu *difficultyMenu;
    QMenu *helpMenu;

    QAction *welcomeAct;
    QAction *grammarAct;
    QAction *translationAct;
    QAction *exitAct;

    QAction *addGrammarAct;
    QAction *addTranslationAct;

    QActionGroup *difficultyGroup;
    QAction *easyAct;
    QAction *mediumAct;
    QAction *hardAct;

    QAction *helpAct;
    QAction *helpExercise;

    QStackedWidget *stack;
    QWidget *welcomePage;
    Grammar *grammarPage;
    Translation *translationPage;

    int currentTimeGrammar;
    int currentTimeTranslation;
    QString currentDifficulty;
    QVector<Question> grammarData;
    QVector<Sentence> translationData;
};

#endif // MAINWINDOW_H
