#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void startTranslation();
    void startGrammar();

    void submitTranslation();
    void submitGrammar();

    void updateTimer();

    void changeDifficulty();

private:
    void setupUI();
    void setupMenu();
    void loadTranslationTask();
    void loadGrammarTask();
    void finishExercise(QString text);

    bool compareAnswers(QString a, QString b);

    // widgets
    QWidget *centralWidget;

    QStackedWidget *stack;

    QWidget *translationPage;
    QWidget *grammarPage;

    QPushButton *translationBtn;
    QPushButton *grammarBtn;

    QLabel *translationQuestion;
    QTextEdit *translationInput;
    QPushButton *translationSubmit;

    QLabel *grammarQuestion;
    QRadioButton *r1;
    QRadioButton *r2;
    QRadioButton *r3;
    QPushButton *grammarSubmit;

    QButtonGroup *group;

    QLabel *scoreLabel;
    QLabel *timerLabel;

    QProgressBar *progressBar;

    // timer
    QTimer *timer;
    int timeLeft;

    // data
    int score;
    int mistakes;
    int currentTask;
    int maxMistakes;
    int totalTasks;

    QString difficulty;

    QVector<QPair<QString, QString>> translationTasks;

    struct GrammarTask
    {
        QString question;
        QString a1;
        QString a2;
        QString a3;
        int correct;
    };

    QVector<GrammarTask> grammarTasks;

};

#endif // MAINWINDOW_H
