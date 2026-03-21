#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QVector>
#include <QStack>
#include <QRandomGenerator>
#include <QColor>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCountChanged(int count);
    void onTicketSelected(QListWidgetItem *item);
    void onTicketDoubleClicked(QListWidgetItem *item);
    void onNextClicked();
    void onPrevClicked();
    void onNameEditReturn();
    void onStatusChanged(int index);
    void updateProgressBars();

private:
    enum Status { Default = 0, Yellow = 1, Green = 2 };
    
    struct Ticket {
        int id;
        QString name;
        Status status;
    };

    void updateTicketView(int index);
    void refreshListColors();
    QColor getStatusColor(Status s);
    QString getStatusText(Status s);
    void resetTickets(int count);
    QSpinBox *spinCount;
    QListWidget *listWidget;
    QGroupBox *groupDetails;
    QLabel *labelNumber;
    QLabel *labelName;
    QLineEdit *editName;
    QComboBox *comboStatus;
    QPushButton *btnNext;
    QPushButton *btnPrev;
    QProgressBar *barTotal;
    QProgressBar *barGreen;
    QVector<Ticket> tickets;
    QStack<int> history;
    int currentIndex;
};

#endif // MAINWINDOW_H