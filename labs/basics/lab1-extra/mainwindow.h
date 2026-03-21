#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QCalendarWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringList>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void onAddOutcome();
    void onDeleteOutcome();
    void onCalculate(); 
    void syncCalendarToDate();
    void syncDateToCalendar();

private:
    QLineEdit *categoryEdit; 
    QDateEdit *birthDateEdit;
    QPushButton *calcButton;
    QLabel *resultLabel;
    
    QListWidget *outcomesList;
    QCalendarWidget *calendar;

    QLineEdit *newOutcomeEdit;
    QPushButton *addBtn;
    QPushButton *delBtn;
};