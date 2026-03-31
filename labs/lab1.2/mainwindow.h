#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QTimer>
#include <QGroupBox>
#include <QToolTip>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QLabel *mainLabel;
    QLabel *questionLabel;
    QLabel *timerLabel;
    QLabel *literLabel;
    QLabel *literLabel1;
    QSpinBox *numberOfEggs;
    QComboBox *typeOfEggs;
    QTimer *timer;
    int timeLeft = 299;
    QPushButton *startButton;
    QPushButton *stopButton;
    QPushButton *nullButton;
    QGroupBox *groupBox1;
    QGroupBox *groupBox2;
    QGroupBox *groupBox3;
    QGroupBox *groupBox4;

private slots:
    void literChange();
    void setSeconds();
    void showToolTip(int index);
    void startTimer();
    void onTimeout();
    void stopTimer();
    void nullTimer();
};
#endif // MAINWINDOW_H
