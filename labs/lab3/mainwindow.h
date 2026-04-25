#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QProgressBar>
#include <QTimer>
#include <QVector>

struct Question {
    bool isGrammar;
    QString text;
    QString answer;
    QStringList options;
    QString hint;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void startExercise();
    void checkAnswer();
    void updateTimer();
    void changeDifficulty();

private:
    void setupUI();
    void showNext();
    void finish(const QString &msg, bool success);

    QVector<Question> questions;
    int currentIdx = 0;
    int mistakes = 0;
    int timeLeft = 60;
    int score = 0;

    QStackedWidget *stack;
    QLabel *qLabel, *timerLabel, *scoreLabel;
    QLineEdit *transInput;
    QButtonGroup *gramGroup;
    QVBoxLayout *optionsLayout;
    QWidget *optWidget;
    QProgressBar *progress;
    QTimer *timer;
};

#endif