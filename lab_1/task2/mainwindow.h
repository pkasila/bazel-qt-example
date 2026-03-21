#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QLCDNumber>
#include <QTimer>
#include <QFrame>
#include <QGridLayout>
#include <QKeyEvent>
#include "game.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onStartClicked();
    void onResetClicked();
    void onSpeedChanged(int index);
    void onScoreUpdated(int score);
    void onGameOver();
    void onPaused(bool paused);
    void updateTimer();
    void onLinesUpdated(int lines);

private:
    void setupUI();
    void setupConnections();
    void updateField();
    void installKeyFilter();

    QPushButton *startButton;
    QPushButton *resetButton;
    QComboBox *speedCombo;
    QLabel *scoreLabel;
    QLabel *linesLabel;
    QFrame *fieldFrame;
    QLCDNumber *lcdScore;
    
    QLabel *cells[20][10];
    
    Game *game;
    QTimer *timer;
    int timerInterval;
};

#endif // MAINWINDOW_H