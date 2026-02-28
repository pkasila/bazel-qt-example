#pragma once
#include "ticket.h"
#include <QtWidgets>
#include <QPushButton>
//#include <bits/stdc++.h>
#include <vector>
#include <deque>


class okno : public QWidget
{
    Q_OBJECT
public:
    okno(QWidget *parent = nullptr);
    void updateticket(int i, QString nm = "");
    void addbilet(int n);
    void updatebars();
protected:
    void closeEvent(QCloseEvent *event) override;
private:

    QVBoxLayout *mainlayout;

    QWidget *topwidget;
    QHBoxLayout *toplayout;

    QWidget *bottomwidget;
    QVBoxLayout *bottomlayout;
    QListWidget *list;

    QProgressBar *yellowbar;
    QProgressBar *greenbar;

    std::deque<int> lasttickets;
    int kolbil = 0;
    std::vector <ticket*> tickets;
    std::vector <QListWidgetItem*> ticketbuttons;


private slots:
    void prevbutton();
    void numtickets(int kol);
    void nextbutton();
};
