#pragma once
#include <QtWidgets>
#include <QPushButton>

class okno;

class ticket : public QWidget
{
    Q_OBJECT
public:
    ticket(int num, okno *m, QWidget *parent);
private:
    friend class okno;
    okno *mama;
    int number;
    QString name;
    int status = 0; // 0-gray, 1-yellow, 2-green
    bool doubleClickFlag = false;
    bool opened = false;
private slots:
    void cl(bool flag);
    void oneclick(bool flag);
    void dcl();
};
