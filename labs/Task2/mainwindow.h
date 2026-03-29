#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCoreApplication>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QProgressBar>
#include <QPixmap>
#include <QCalendarWidget>
#include <QLCDNumber>


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void calculateResult();
    void updateProgress();
    void updateTimer();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    QPixmap backgroundPixmap;
    bool checkFieldsFilled();

    QComboBox *mathAnalysisComboBox;
    QComboBox *algebraComboBox;
    QComboBox *programmingComboBox;
    QComboBox *historyComboBox;
    QComboBox *englishComboBox;
    QComboBox *discreteMathComboBox;

    QCheckBox *olympiadMathCheckBox;
    QCheckBox *olympiadInformaticsCheckBox;
    QCheckBox *olympiadAstronomyCheckBox;

    QSpinBox *diligenceSpinBox;
    QSpinBox *memorySpinBox;
    QSpinBox *thinkingSpeedSpinBox;
    QSpinBox *neatnessSpinBox;

    QPushButton *calculateButton;
    QLabel *resultLabel;
    QProgressBar *progressBar;
    QHBoxLayout *topLayout;
    QTimer *timer;
    QLabel *timerLabel;
    QLCDNumber *timerDisplay;
    QCalendarWidget *miniCalendar;
    int elapsedTime = 0;
};

#endif // MAINWINDOW_H
