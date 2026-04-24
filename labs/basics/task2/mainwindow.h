#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QCalendarWidget>
#include <QLCDNumber>
#include <QLabel>
#include <QMap>
#include <QDate>
#include <QListWidget>
#include <QStringList>

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();
    void calculateStress();
    void addTaskToPlan();
    void checkDateUrgency(const QDate &date);
    void refreshTaskList(int week);

    QLineEdit *m_taskEdit;
    QSlider *m_difficultySlider;
    QCheckBox *m_urgentBox;
    QPushButton *m_addBtn;

    QCalendarWidget *m_calendar;
    QLCDNumber *m_lcdDisplay;

    QLabel *m_statusLabel;
    QListWidget *m_taskList;
    QMap<int, int> m_weeklyStress;
    QMap<int, QStringList> m_weeklyTaskNames; 
};