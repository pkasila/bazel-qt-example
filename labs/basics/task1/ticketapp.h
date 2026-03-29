#ifndef TICKETAPP_H
#define TICKETAPP_H

#include <QWidget>
#include <QSpinBox>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QLineEdit>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRandomGenerator>
#include <QMap>

class TicketApp : public QWidget {
    Q_OBJECT

private:
    QSpinBox *ticketCount;
    QListWidget *ticketList;
    QLabel *questionLabel;
    QLineEdit *nameEdit;
    QPushButton *nextButton;
    QPushButton *prevButton;
    QProgressBar *totalProgress;
    QProgressBar *greenProgress;
    QMap<int, QString> ticketNames;
    QMap<int, QString> ticketStatus;
    int currentIndex = -1;

    void updateProgress();

public:
    TicketApp(QWidget *parent = nullptr);

private slots:
    void updateTicketList(int count);
    void selectTicket(QListWidgetItem *item);
    void updateTicketName();
    void nextTicket();
    void prevTicket();
    void toggleTicketStatus(QListWidgetItem *item);
};

#endif // TICKETAPP_H
