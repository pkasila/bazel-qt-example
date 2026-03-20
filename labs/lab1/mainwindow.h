#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QGroupBox>
#include <QVector>
#include <QStack>

struct Ticket {
    int number;
    QString name;
    int status;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void setupTickets(int count);
    void onTicketClicked(QListWidgetItem *item);
    void onTicketDoubleClicked(QListWidgetItem *item);
    void updateTicketName();
    void onStatusChanged(int index);
    void nextRandomTicket();
    void previousTicket();

private:
    void updateView();
    void updateProgress();
    void displayTicket(int index);

    QSpinBox *countSpinBox;
    QListWidget *ticketList;
    QProgressBar *totalProgress;
    QProgressBar *greenProgress;
    
    QGroupBox *detailBox;
    QLabel *lblNumber;
    QLabel *lblName;
    QLineEdit *nameEdit;
    QComboBox *statusCombo;
    QPushButton *btnNext;
    QPushButton *btnPrev;

    QVector<Ticket> tickets;
    QStack<int> history;
    int currentIndex = -1;
};

#endif