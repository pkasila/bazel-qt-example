#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QProgressBar>
#include <QPushButton>
#include "ticketlist.h"
#include "infobox.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSpinBox *getSpinBox() { return count; }
    void updateProgress();

private slots:
    void updateInfoBox(TicketItem *ticket);

private:
    QSpinBox *count;
    TicketList *list;
    InfoBox *questionView;
    QPushButton *button;
    QPushButton *buttonprev;
    QProgressBar *totalprogress;
    QProgressBar *progress;
};

#endif // MAINWINDOW_H
